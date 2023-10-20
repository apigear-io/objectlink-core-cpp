#include <catch2/catch.hpp>

#include "olink/core/types.h"
#include "olink/core/protocol.h"

#include <string>
#include "nlohmann/json.hpp"


using namespace ApiGear::ObjectLink;


TEST_CASE("protocol")
{
    std::string name = "demo.Calc";
    nlohmann::json props = {{ "count", 0 }};
    int value = 1;
    nlohmann::json args = {1, 2};
    int requestId = 1;
    MsgType msgType = MsgType::Invoke;
    std::string error = "failed";

    SECTION("link") {
        auto msg = Protocol::linkMessage(name);
        REQUIRE(msg.message[0] == MsgType::Link);
        REQUIRE(msg.message[1] == name);
    }
    SECTION("unlink") {
        auto msg = Protocol::unlinkMessage(name);
        REQUIRE(msg.message[0] == MsgType::Unlink);
        REQUIRE(msg.message[1] == name);
    }
    SECTION("init") {
        auto msg = Protocol::initMessage(name, { props });
        REQUIRE(msg.message[0] == MsgType::Init);
        REQUIRE(msg.message[1] == name);
        REQUIRE(msg.message[2] == props);
    }
    SECTION("setProperty") {
        auto msg = Protocol::setPropertyMessage(name, { value });
        REQUIRE(msg.message[0] == MsgType::SetProperty);
        REQUIRE(msg.message[1] == name);
        REQUIRE(msg.message[2] == value);
    }
    SECTION("propertyChange") {
        auto msg = Protocol::propertyChangeMessage(name, { value });
        REQUIRE(msg.message[0] == MsgType::PropertyChange);
        REQUIRE(msg.message[1] == name);
        REQUIRE(msg.message[2] == value);
    }
    SECTION("invoke") {
        auto msg = Protocol::invokeMessage(requestId, name, { args });
        REQUIRE(msg.message[0] == MsgType::Invoke);
        REQUIRE(msg.message[1] == requestId);
        REQUIRE(msg.message[2] == name);
        REQUIRE(msg.message[3] == args);
    }
    SECTION("invokeReply") {
        auto msg = Protocol::invokeReplyMessage(requestId, name, { value });
        REQUIRE(msg.message[0] == MsgType::InvokeReply);
        REQUIRE(msg.message[1] == requestId);
        REQUIRE(msg.message[2] == name);
        REQUIRE(msg.message[3] == value);
    }
    SECTION("signal") {
        auto msg = Protocol::signalMessage(name, { args });
        REQUIRE(msg.message[0] == MsgType::Signal);
        REQUIRE(msg.message[1] == name);
        REQUIRE(msg.message[2] == args);
    }
    SECTION("error") {
        auto msg = Protocol::errorMessage(msgType, requestId, error);
        REQUIRE(msg.message[0] == MsgType::Error);
        REQUIRE(msg.message[1] == msgType);
        REQUIRE(msg.message[2] == requestId);
        REQUIRE(msg.message[3] == error);
    }
}
