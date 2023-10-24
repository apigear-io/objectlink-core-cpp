#include <catch2/catch.hpp>

#include "olink/core/types.h"
#include "olink/core/protocol.h"

#include <string>
#include "nlohmann/json.hpp"


using namespace ApiGear::ObjectLink;


TEST_CASE("protocol")
{
    std::string name = "demo.Calc";
    int initialValueForCount = 0;
    std::string initPropertyName = "count";
    auto initialProperties = argumentsToContent(toInitialProperty("count", initialValueForCount));
    int value = 1;
    int int_argument = 1;
    std::string string_argument = "some";
    int requestId = 1;
    MsgType msgType = MsgType::Invoke;
    std::string error = "failed";

    MsgType created_type;
    std::string created_id;
    OLinkContent messageContent;
    int expected_requestId = 9999; //Different than requestId, will be overridden with correct value.

    SECTION("link") {
        auto msg = Protocol::linkMessage(name);
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        REQUIRE(created_type == MsgType::Link);
        REQUIRE(created_id == name);
    }
    SECTION("unlink") {
        auto msg = Protocol::unlinkMessage(name);
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        REQUIRE(created_type == MsgType::Unlink);
        REQUIRE(created_id == name);
    }
    SECTION("init") {
        auto msg = Protocol::initMessage(name, initialProperties);
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        msgReader.read(messageContent);
        OLinContentStreamReader contentReader(messageContent);
        InitialProperty expectedInitialProperty;
        contentReader.read(expectedInitialProperty);
        int expectedInitValue =  9999;
        readValue(expectedInitialProperty, expectedInitValue);

        REQUIRE(created_type == MsgType::Init);
        REQUIRE(created_id == name);
        REQUIRE(expectedInitialProperty.propertyName == initPropertyName);
        REQUIRE(expectedInitValue == initialValueForCount);
    }
    SECTION("setProperty") {
        auto msg = Protocol::setPropertyMessage(name, propertyToContent(value));
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        msgReader.read(messageContent);
        int expectedValue = 9999;
        readValue(messageContent, expectedValue);
        REQUIRE(created_type == MsgType::SetProperty);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("propertyChange") {
        auto msg = Protocol::propertyChangeMessage(name, propertyToContent( value ));
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        msgReader.read(messageContent);
        int expectedValue = 9999;
        readValue(messageContent, expectedValue);
        REQUIRE(created_type == MsgType::PropertyChange);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("invoke") {
        auto msg = Protocol::invokeMessage(requestId, name, argumentsToContent(int_argument, string_argument));
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(expected_requestId);
        msgReader.read(created_id);
        msgReader.read(messageContent);
        OLinContentStreamReader contentReader(messageContent);
        int expected_int = 9999;
        std::string expected_string = "this should be filled with expected content";
        contentReader.read(expected_int);
        contentReader.read(expected_string);
        REQUIRE(created_type == MsgType::Invoke);
        REQUIRE(created_id == name);
        REQUIRE(expected_requestId == requestId);
        REQUIRE(expected_int == int_argument);
        REQUIRE(expected_string == string_argument);
    }
    SECTION("invokeReply") {
        auto msg = Protocol::invokeReplyMessage(requestId, name, invokeReturnValue( value ));
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(expected_requestId);
        msgReader.read(created_id);
        msgReader.read(messageContent);
    
        int expectedValue = 9999;
        readValue(messageContent, expectedValue);

        REQUIRE(created_type == MsgType::InvokeReply);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("signal") {
        auto msg = Protocol::signalMessage(name, argumentsToContent(int_argument, string_argument));
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(created_id);
        msgReader.read(messageContent);
        OLinContentStreamReader contentReader(messageContent);
        int expected_int = 9999;
        std::string expected_string = "this should be filled with expected content";
        contentReader.read(expected_int);
        contentReader.read(expected_string);
        REQUIRE(created_type == MsgType::Signal);
        REQUIRE(created_id == name);
        REQUIRE(expected_int == int_argument);
        REQUIRE(expected_string == string_argument);
    }
    SECTION("error") {
        auto msg = Protocol::errorMessage(msgType, requestId, error);
        REQUIRE(msgType != MsgType::Error);
        MsgType expected_msgTypeWithError = MsgType::Error;// should be overridden
        std::string expected_error = "";
        auto msgReader = OLinkMessageStreamReader(msg);
        msgReader.read(created_type);
        msgReader.read(expected_msgTypeWithError);
        msgReader.read(expected_requestId);
        msgReader.read(expected_error);

        REQUIRE(created_type == MsgType::Error);
        REQUIRE(expected_msgTypeWithError == msgType);
        REQUIRE(expected_requestId == requestId);
        REQUIRE(expected_error == error);
    }
}
