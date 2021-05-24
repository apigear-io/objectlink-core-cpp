#include <catch2/catch.hpp>

#include "olink/core/types.h"
#include "olink/core/protocol.h"
#include "nlohmann/json.hpp"

#include <list>
#include <string>
#include "mocksink.hpp"
#include "mocksource.hpp"
#include "olink/consolelogger.h"

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;

const std::string name = "demo.Calc";
const json props = {{ "count", 0 }};
const int value = 1;
const json args = json({1, 2});
const int requestId = 1;
const MsgType msgType = MsgType::INVOKE;
const std::string error = "failed";

TEST_CASE("protocol")
{
    ConsoleLogger log;
    MockSink sink;
    MockSource source;
    ClientNode client;
    RemoteNode remote;
    client.onLog(log.logFunc());
    remote.onLog(log.logFunc());
    SECTION("link") {
        json msg = Protocol::linkMessage(name);
        REQUIRE(msg[0] == MsgType::LINK);
        REQUIRE(msg[1] == name);
    }
    SECTION("unlink") {
        json msg = Protocol::unlinkMessage(name);
        REQUIRE(msg[0] == MsgType::UNLINK);
        REQUIRE(msg[1] == name);
    }
    SECTION("init") {
        json msg = Protocol::initMessage(name, props);
        REQUIRE(msg[0] == MsgType::INIT);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == props);
    }
    SECTION("setProperty") {
        json msg = Protocol::setPropertyMessage(name, value);
        REQUIRE(msg[0] == MsgType::SET_PROPERTY);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == value);
    }
    SECTION("propertyChange") {
        json msg = Protocol::propertyChangeMessage(name, value);
        REQUIRE(msg[0] == MsgType::PROPERTY_CHANGE);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == value);
    }
    SECTION("invoke") {
        json msg = Protocol::invokeMessage(requestId, name, args);
        REQUIRE(msg[0] == MsgType::INVOKE);
        REQUIRE(msg[1] == requestId);
        REQUIRE(msg[2] == name);
        REQUIRE(msg[3] == args);
    }
    SECTION("invokeReply") {
        json msg = Protocol::invokeReplyMessage(requestId, name, value);
        REQUIRE(msg[0] == MsgType::INVOKE_REPLY);
        REQUIRE(msg[1] == requestId);
        REQUIRE(msg[2] == name);
        REQUIRE(msg[3] == value);
    }
    SECTION("signal") {
        json msg = Protocol::signalMessage(name, args);
        REQUIRE(msg[0] == MsgType::SIGNAL);
        REQUIRE(msg[1] == name);
        REQUIRE(msg[2] == args);
    }
    SECTION("error") {
        json msg = Protocol::errorMessage(msgType, requestId, error);
        REQUIRE(msg[0] == MsgType::ERROR);
        REQUIRE(msg[1] == msgType);
        REQUIRE(msg[2] == requestId);
        REQUIRE(msg[3] == error);
    }
}
