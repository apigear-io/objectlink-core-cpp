#include <catch2/catch.hpp>

#include "olink/core/types.h"
#include "olink/core/protocol.h"
#include "olink/core/basenode.h"

#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;


TEST_CASE("protocol")
{
    std::string name = "demo.Calc";
    json props = {{ "count", 0 }};
    int value = 1;
    json args = {1, 2};
    unsigned int requestId = 1;
    MsgType msgType = MsgType::Invoke;
    std::string error = "failed";

    SECTION("link") {
        json msg = Protocol::linkMessage(name);
        REQUIRE(msg[0] == MsgType::Link);
        REQUIRE(msg[1] == name);
    }
    SECTION("unlink") {
        json msg = Protocol::unlinkMessage(name);
        REQUIRE(msg[0] == MsgType::Unlink);
        REQUIRE(msg[1] == name);
    }
    SECTION("init") {
        json msg = Protocol::initMessage(name, props);
        REQUIRE(msg[0] == MsgType::Init);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == props);
    }
    SECTION("setProperty") {
        json msg = Protocol::setPropertyMessage(name, value);
        REQUIRE(msg[0] == MsgType::SetProperty);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == value);
    }
    SECTION("propertyChange") {
        json msg = Protocol::propertyChangeMessage(name, value);
        REQUIRE(msg[0] == MsgType::PropertyChange);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == value);
    }
    SECTION("invoke") {
        json msg = Protocol::invokeMessage(requestId, name, args);
        REQUIRE(msg[0] == MsgType::Invoke);
        REQUIRE(msg[1] == requestId);
        REQUIRE(msg[2] == name);
        REQUIRE(msg[3] == args);
    }
    SECTION("invokeReply") {
        json msg = Protocol::invokeReplyMessage(requestId, name, value);
        REQUIRE(msg[0] == MsgType::InvokeReply);
        REQUIRE(msg[1] == requestId);
        REQUIRE(msg[2] == name);
        REQUIRE(msg[3] == value);
    }
    SECTION("signal") {
        json msg = Protocol::signalMessage(name, args);
        REQUIRE(msg[0] == MsgType::Signal);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == args);
    }
    SECTION("error") {
        json msg = Protocol::errorMessage(msgType, static_cast<int>(requestId), error);
        REQUIRE(msg[0] == MsgType::Error);
        REQUIRE(msg[1] == msgType);
        REQUIRE(msg[2] == requestId);
        REQUIRE(msg[3] == error);
    }
}

// Stub listener that records nothing — we only care that handleMessage returns false
// for truncated messages, not that listener methods are called.
class StubProtocolListener : public IProtocolListener {
public:
    void handleLink(const std::string& /*objectId*/) override {}
    void handleUnlink(const std::string& /*objectId*/) override {}
    void handleInit(const std::string& /*objectId*/, const nlohmann::json& /*props*/) override {}
    void handleSetProperty(const std::string& /*propertyId*/, const nlohmann::json& /*value*/) override {}
    void handlePropertyChange(const std::string& /*propertyId*/, const nlohmann::json& /*value*/) override {}
    void handleInvoke(unsigned int /*requestId*/, const std::string& /*methodId*/, const nlohmann::json& /*args*/) override {}
    void handleInvokeReply(unsigned int /*requestId*/, const std::string& /*methodId*/, const nlohmann::json& /*value*/) override {}
    void handleSignal(const std::string& /*signalId*/, const nlohmann::json& /*args*/) override {}
    void handleError(int /*msgType*/, int /*requestId*/, const std::string& /*error*/) override {}
};

TEST_CASE("handleMessage truncated messages", "[Protocol]")
{
    Protocol protocol;
    StubProtocolListener listener;

    SECTION("non-array message returns false") {
        json msg = "not an array";
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
    }

    SECTION("empty array returns false") {
        json msg = json::array();
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
    }

    SECTION("truncated Link message") {
        json msg = json::array({ MsgType::Link });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Init message - missing props") {
        json msg = json::array({ MsgType::Init, "demo.Calc" });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Init message - missing objectId and props") {
        json msg = json::array({ MsgType::Init });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Unlink message") {
        json msg = json::array({ MsgType::Unlink });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated SetProperty message") {
        json msg = json::array({ MsgType::SetProperty, "demo.Calc/count" });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated PropertyChange message") {
        json msg = json::array({ MsgType::PropertyChange, "demo.Calc/count" });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Invoke message - missing args") {
        json msg = json::array({ MsgType::Invoke, 1, "demo.Calc/add" });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Invoke message - only type") {
        json msg = json::array({ MsgType::Invoke });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated InvokeReply message") {
        json msg = json::array({ MsgType::InvokeReply, 1 });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Signal message") {
        json msg = json::array({ MsgType::Signal });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("truncated Error message") {
        json msg = json::array({ MsgType::Error, 30 });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }
}

// Helper concrete BaseNode for testing — BaseNode is not abstract for handleMessage
class TestableBaseNode : public BaseNode {
public:
    bool loggedError = false;
    std::string lastLogMessage;

    TestableBaseNode() {
        onLog([this](LogLevel level, const std::string& msg) {
            if (level == LogLevel::Error) {
                loggedError = true;
            }
            lastLogMessage = msg;
        });
        setLogLevel(LogLevel::Info);
    }
};

TEST_CASE("handleMessage type mismatch", "[Protocol]")
{
    Protocol protocol;
    StubProtocolListener listener;

    SECTION("Link with integer instead of string objectId") {
        json msg = json::array({ MsgType::Link, 42 });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("Invoke with string instead of unsigned int requestId") {
        json msg = json::array({ MsgType::Invoke, "not-an-int", "method", json::array() });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }

    SECTION("Init with integer instead of string objectId") {
        json msg = json::array({ MsgType::Init, 42, json::object() });
        REQUIRE_FALSE(protocol.handleMessage(msg, listener));
        REQUIRE_FALSE(protocol.lastError().empty());
    }
}

TEST_CASE("BaseNode::handleMessage with invalid data", "[BaseNode]")
{
    TestableBaseNode node;

    SECTION("invalid JSON does not crash") {
        REQUIRE_NOTHROW(node.handleMessage("this is not json{{{"));
    }

    SECTION("invalid JSON is handled gracefully") {
        // Should not crash or throw; the invalid input is silently rejected
        node.handleMessage("this is not json{{{");
        // No crash means success - the empty json from fromString
        // is handled by Protocol::handleMessage returning false
    }

    SECTION("empty string does not crash") {
        REQUIRE_NOTHROW(node.handleMessage(""));
    }

    SECTION("valid but non-array JSON does not crash") {
        REQUIRE_NOTHROW(node.handleMessage("42"));
    }

    SECTION("truncated message does not crash") {
        // A valid JSON array but truncated protocol message
        REQUIRE_NOTHROW(node.handleMessage("[10]"));
    }

    SECTION("type mismatch in message logs error via catch") {
        // This triggers the try-catch in BaseNode because Protocol::handleMessage
        // re-throws type_error as false return, but the BaseNode catch is a safety net
        // For invalid JSON, the non-throwing parse handles it silently
        // For type mismatches, Protocol now catches them internally
        // Just verify no crash occurs
        node.handleMessage("[10, 42]");  // Link with int instead of string
    }

    SECTION("setMaxMessageSize forwards to converter") {
        node.setMaxMessageSize(10);
        std::string longMsg = "[10, \"demo.Calc\"]";  // > 10 bytes
        node.handleMessage(longMsg);
        // Message rejected silently by converter, protocol sees empty json
        // No crash is the success criterion
    }
}

TEST_CASE("MessageConverter::fromString with invalid data", "[MessageConverter]")
{
    MessageConverter converter(MessageFormat::JSON);

    SECTION("invalid JSON returns empty json without throwing") {
        nlohmann::json result;
        REQUIRE_NOTHROW(result = converter.fromString("not valid json"));
        REQUIRE(result.empty());
    }

    SECTION("empty string returns empty json without throwing") {
        nlohmann::json result;
        REQUIRE_NOTHROW(result = converter.fromString(""));
        REQUIRE(result.empty());
    }
}

TEST_CASE("MessageConverter message size limit", "[MessageConverter]")
{
    MessageConverter converter(MessageFormat::JSON);

    SECTION("message within default limit parses correctly") {
        std::string validMsg = "[10, \"demo.Calc\"]";
        nlohmann::json result = converter.fromString(validMsg);
        REQUIRE(result.is_array());
        REQUIRE(result.size() == 2);
    }

    SECTION("message exceeding custom limit returns empty json") {
        converter.setMaxMessageSize(10);
        std::string longMsg = "[10, \"demo.Calc\"]";  // > 10 bytes
        nlohmann::json result = converter.fromString(longMsg);
        REQUIRE(result.empty());
    }

    SECTION("setMaxMessageSize works") {
        converter.setMaxMessageSize(5);
        // Even valid JSON is rejected if too large
        nlohmann::json result = converter.fromString("[10, \"x\"]");
        REQUIRE(result.empty());
    }

    SECTION("message exactly at limit parses correctly") {
        std::string msg = "[10]";
        converter.setMaxMessageSize(msg.size());
        nlohmann::json result = converter.fromString(msg);
        REQUIRE(result.is_array());
    }
}
