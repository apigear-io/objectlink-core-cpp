#include <catch2/catch.hpp>

#include "olink/core/types.h"
#include "olink/core/protocol.h"

#include <string>
#include "nlohmann/json.hpp"

#include "olink/core/defaultmessageserializer.h"
#include "olink/core/defaultcontentserializer.h"

using namespace ApiGear::ObjectLink;

namespace ContentSerializer = ApiGear::ObjectLink::NlohmannSerializer;

TEST_CASE("protocol")
{
    auto serializer = std::make_shared<ApiGear::ObjectLink::NlohmannMessageSerializer>();
    std::string name = "demo.Calc";
    int initialValueForCount = 0;
    std::string initPropertyName = "count";
    auto initialProperties = ContentSerializer::Arguments::serialize(ContentSerializer::toInitialProperty("count", initialValueForCount));
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
        auto msg = Protocol::linkMessage(*(serializer->createWriter()), name);
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        REQUIRE(created_type == MsgType::Link);
        REQUIRE(created_id == name);
    }
    SECTION("unlink") {
        auto msg = Protocol::unlinkMessage(*(serializer->createWriter()), name);
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        REQUIRE(created_type == MsgType::Unlink);
        REQUIRE(created_id == name);
    }
    SECTION("init") {
        auto msg = Protocol::initMessage(*(serializer->createWriter()), name, initialProperties);
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);
        ContentSerializer::Arguments::Deserializer contentReader(messageContent);
        InitialProperty expectedInitialProperty;
        contentReader.getNext(expectedInitialProperty);
        int expectedInitValue =  9999;
        ContentSerializer::fromInitialProperty(expectedInitialProperty, expectedInitValue);

        REQUIRE(created_type == MsgType::Init);
        REQUIRE(created_id == name);
        REQUIRE(expectedInitialProperty.propertyName == initPropertyName);
        REQUIRE(expectedInitValue == initialValueForCount);
    }
    SECTION("setProperty") {
        auto msg = Protocol::setPropertyMessage(*(serializer->createWriter()), name, ContentSerializer::Value::serialize(value));
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);
        int expectedValue = 9999;
        ContentSerializer::Value::deserialize(messageContent, expectedValue);
        REQUIRE(created_type == MsgType::SetProperty);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("propertyChange") {
        auto msg = Protocol::propertyChangeMessage(*(serializer->createWriter()), name, ContentSerializer::Value::serialize( value ));
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);
        int expectedValue = 9999;
        ContentSerializer::Value::deserialize(messageContent, expectedValue);
        REQUIRE(created_type == MsgType::PropertyChange);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("invoke") {
        auto msg = Protocol::invokeMessage(*(serializer->createWriter()), requestId, name, ContentSerializer::Arguments::serialize(int_argument, string_argument));
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(expected_requestId);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);

        REQUIRE(created_type == MsgType::Invoke);
        REQUIRE(created_id == name);
        REQUIRE(expected_requestId == requestId);

        ContentSerializer::Arguments::Deserializer contentReader(messageContent);
        int expected_int = 9999;
        std::string expected_string = "this should be filled with expected content";
        contentReader.getNext(expected_int);
        contentReader.getNext(expected_string);

        REQUIRE(expected_int == int_argument);
        REQUIRE(expected_string == string_argument);
    }
    SECTION("invokeReply") {
        auto msg = Protocol::invokeReplyMessage(*(serializer->createWriter()), requestId, name, ContentSerializer::Value::serialize( value ));
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(expected_requestId);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);
    
        int expectedValue = 9999;
        ContentSerializer::Value::deserialize(messageContent, expectedValue);

        REQUIRE(created_type == MsgType::InvokeReply);
        REQUIRE(created_id == name);
        REQUIRE(expectedValue == value);
    }
    SECTION("signal") {
        auto msg = Protocol::signalMessage(*(serializer->createWriter()), name, ContentSerializer::Arguments::serialize(int_argument, string_argument));
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(created_id);
        msgReader->readNext(messageContent);
        ContentSerializer::Arguments::Deserializer contentReader(messageContent);
        int expected_int = 9999;
        std::string expected_string = "this should be filled with expected content";
        contentReader.getNext(expected_int);
        contentReader.getNext(expected_string);
        REQUIRE(created_type == MsgType::Signal);
        REQUIRE(created_id == name);
        REQUIRE(expected_int == int_argument);
        REQUIRE(expected_string == string_argument);
    }
    SECTION("error") {
        auto msg = Protocol::errorMessage(*(serializer->createWriter()), msgType, requestId, error);
        REQUIRE(msgType != MsgType::Error);
        MsgType expected_msgTypeWithError = MsgType::Error;// should be overridden
        std::string expected_error = "";
        auto msgReader = serializer->createReader(msg);
        msgReader->readNext(created_type);
        msgReader->readNext(expected_msgTypeWithError);
        msgReader->readNext(expected_requestId);
        msgReader->readNext(expected_error);

        REQUIRE(created_type == MsgType::Error);
        REQUIRE(expected_msgTypeWithError == msgType);
        REQUIRE(expected_requestId == requestId);
        REQUIRE(expected_error == error);
    }
}
