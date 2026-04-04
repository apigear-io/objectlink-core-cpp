// Standalone test compiled with -fno-exceptions to verify the library
// compiles and works correctly without C++ exceptions.
// Does NOT use Catch2 (which requires exceptions).

#include "olink/core/types.h"
#include "olink/core/protocol.h"
#include "olink/core/basenode.h"

#include <cassert>
#include <cstdio>
#include <string>

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;

// Stub listener — we only care about return values, not handler calls
class NoExceptStubListener : public IProtocolListener {
public:
    void handleLink(const std::string&) override {}
    void handleUnlink(const std::string&) override {}
    void handleInit(const std::string&, const nlohmann::json&) override {}
    void handleSetProperty(const std::string&, const nlohmann::json&) override {}
    void handlePropertyChange(const std::string&, const nlohmann::json&) override {}
    void handleInvoke(unsigned int, const std::string&, const nlohmann::json&) override {}
    void handleInvokeReply(unsigned int, const std::string&, const nlohmann::json&) override {}
    void handleSignal(const std::string&, const nlohmann::json&) override {}
    void handleError(int, int, const std::string&) override {}
};

static void test_converter_invalid_input()
{
    MessageConverter converter(MessageFormat::JSON);

    json result = converter.fromString("not valid json");
    assert(result.empty());

    result = converter.fromString("");
    assert(result.empty());
}

static void test_converter_size_limit()
{
    MessageConverter converter(MessageFormat::JSON);
    converter.setMaxMessageSize(5);

    json result = converter.fromString("[10, \"demo.Calc\"]");
    assert(result.empty());
}

static void test_protocol_valid_messages()
{
    Protocol protocol;
    NoExceptStubListener listener;

    json linkMsg = Protocol::linkMessage("demo.Calc");
    assert(protocol.handleMessage(linkMsg, listener));

    json initMsg = Protocol::initMessage("demo.Calc", json::object({{"count", 0}}));
    assert(protocol.handleMessage(initMsg, listener));

    json invokeMsg = Protocol::invokeMessage(1, "demo.Calc/add", json::array({1, 2}));
    assert(protocol.handleMessage(invokeMsg, listener));

    json signalMsg = Protocol::signalMessage("demo.Calc/valueChanged", json::array({42}));
    assert(protocol.handleMessage(signalMsg, listener));
}

static void test_protocol_truncated_messages()
{
    Protocol protocol;
    NoExceptStubListener listener;

    assert(!protocol.handleMessage(json::array(), listener));
    assert(!protocol.handleMessage(json("not an array"), listener));
    assert(!protocol.handleMessage(json::array({MsgType::Link}), listener));
    assert(!protocol.handleMessage(json::array({MsgType::Init, "obj"}), listener));
    assert(!protocol.handleMessage(json::array({MsgType::Invoke}), listener));
    assert(!protocol.handleMessage(json::array({MsgType::Signal}), listener));
    assert(!protocol.handleMessage(json::array({MsgType::Error, 30}), listener));
}

static void test_protocol_type_mismatches()
{
    Protocol protocol;
    NoExceptStubListener listener;

    // Non-integer message type
    assert(!protocol.handleMessage(json::array({"not_int", "obj"}), listener));
    assert(!protocol.lastError().empty());

    // Link with integer instead of string objectId
    assert(!protocol.handleMessage(json::array({MsgType::Link, 42}), listener));
    assert(!protocol.lastError().empty());

    // Invoke with string instead of number requestId
    assert(!protocol.handleMessage(json::array({MsgType::Invoke, "bad", "method", json::array()}), listener));
    assert(!protocol.lastError().empty());

    // Init with integer instead of string objectId
    assert(!protocol.handleMessage(json::array({MsgType::Init, 42, json::object()}), listener));
    assert(!protocol.lastError().empty());

    // Error with string instead of integer msgType
    assert(!protocol.handleMessage(json::array({MsgType::Error, "bad", 0, "err"}), listener));
    assert(!protocol.lastError().empty());
}

static void test_basenode_invalid_input()
{
    BaseNode node;

    // Invalid JSON
    node.handleMessage("this is not json{{{");
    // Empty string
    node.handleMessage("");
    // Valid JSON, non-array
    node.handleMessage("42");
    // Truncated message
    node.handleMessage("[10]");
    // Type mismatch
    node.handleMessage("[10, 42]");
}

static void test_basenode_max_message_size()
{
    BaseNode node;
    node.setMaxMessageSize(10);
    // Message exceeds limit — silently rejected
    node.handleMessage("[10, \"demo.Calc\"]");
}

int main()
{
    std::printf("test_converter_invalid_input... ");
    test_converter_invalid_input();
    std::printf("OK\n");

    std::printf("test_converter_size_limit... ");
    test_converter_size_limit();
    std::printf("OK\n");

    std::printf("test_protocol_valid_messages... ");
    test_protocol_valid_messages();
    std::printf("OK\n");

    std::printf("test_protocol_truncated_messages... ");
    test_protocol_truncated_messages();
    std::printf("OK\n");

    std::printf("test_protocol_type_mismatches... ");
    test_protocol_type_mismatches();
    std::printf("OK\n");

    std::printf("test_basenode_invalid_input... ");
    test_basenode_invalid_input();
    std::printf("OK\n");

    std::printf("test_basenode_max_message_size... ");
    test_basenode_max_message_size();
    std::printf("OK\n");

    std::printf("\nAll no-exceptions tests passed.\n");
    return 0;
}
