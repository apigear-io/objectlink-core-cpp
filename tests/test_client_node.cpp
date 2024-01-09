
#include <catch2/catch.hpp>

#include "olink/iobjectsink.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"
#include "olink/core/types.h"
#include "olink/core/defaultmessageserializer.h"
#include "olink/core/defaultcontentserializer.h"

#include "mocks.h"
#include "matchers.h"

namespace ContentSerializer = ApiGear::ObjectLink::NlohmannSerializer;

namespace {
    // helper constants used in test.
    std::string sink1Id = "tests.sink1";
    std::string sink2Id = "tests.sink2";
    std::string propertyName = "exampleProprety";
    std::string stringProperty = "some string";
    auto serializer = std::make_shared<ApiGear::ObjectLink::NlohmannMessageSerializer>();
    int someIntValue = 9;
    bool someBoolValue= false;
    auto propertyValue = ContentSerializer::Value::serialize(8);
    auto otherPropertyValue = ContentSerializer::Value::serialize(115);
    ApiGear::ObjectLink::OLinkContent exampleInitProperties = ContentSerializer::Arguments::serialize(
        ContentSerializer::toInitialProperty(propertyName, stringProperty),
        ContentSerializer::toInitialProperty("property2", someIntValue),
        ContentSerializer::toInitialProperty("property3", someBoolValue));
    std::string methodName = "exampleMethod";
    std::string signalName = "exampleSingal";
    ApiGear::ObjectLink::OLinkContent exampleArguments = ContentSerializer::Arguments::serialize(stringProperty, 9, false);

    // Helper function which gets from a InvokeMessage a requestId given by clientNode.
    int retrieveRequestId(const std::string& networkMessage)
    {
        const auto& requestMessage = serializer->fromNetworkFormat(networkMessage);
        REQUIRE(requestMessage.message[0].get<int>() == static_cast<int>(ApiGear::ObjectLink::MsgType::Invoke));
        int result = requestMessage.message[1].get<int>();
        return result;
    };

}

TEST_CASE("Client Node")
{

    // Objects required for most tests
    
    // Sink mocks that always return associated with them ids.
    auto sink1 = std::make_shared<SinkObjectMock>();
    auto sink2 = std::make_shared<SinkObjectMock>();
    ALLOW_CALL(*sink1, olinkObjectName()).RETURN(sink1Id);
    ALLOW_CALL(*sink2, olinkObjectName()).RETURN(sink2Id);
    // Output mock to monitor written messages.
    OutputMock outputMock;

    ApiGear::ObjectLink::ClientRegistry registry;

    // Test node kept as ptr to destroy before going out of scope of test. Allows testing destructor.
    auto testedNode = ApiGear::ObjectLink::ClientNode::create(registry);
    // Setting up an output for sending messages. This function must be set by user.
    testedNode->onWrite([&outputMock](const auto& msg){outputMock.writeMessage(msg); }, serializer);

    SECTION("Typical setup and tear down scenario - the node ends life before the sinks, client node need to unlink sinks")
    {
        registry.addSink(sink2);
        registry.addSink(sink1);

        // link sinks
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink2Id))));
        testedNode->linkRemote(sink1Id);
        testedNode->linkRemote(sink2Id);
        REQUIRE(registry.getNode(sink1Id).lock() == testedNode);
        REQUIRE(registry.getNode(sink2Id).lock() == testedNode);

        // Expectations on dtor
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink2Id))));

        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(*sink2, olinkOnRelease());
        
        testedNode->unlinkRemote(sink1Id);
        testedNode->unlinkRemote(sink2Id);
        REQUIRE(registry.getNode(sink1Id).lock() == nullptr);
        REQUIRE(registry.getNode(sink2Id).lock() == nullptr);

        testedNode.reset();
    }

    SECTION("Scenario sinks is deleted, before removing from registry and unlinking")
    {
        auto sink3 = std::make_shared<SinkObjectMock>();
        std::string sink3Id = "tests.sink3";
        ALLOW_CALL(*sink3, olinkObjectName()).RETURN(sink3Id);

        registry.addSink(sink3);

        sink3.reset();
        // Although it is safe to use, it still needs to be removed from registry,
        // and the unlink wasn't sent, so the server still sends the messages.
        // But the node will not pass it to node would like to send unlink message
        REQUIRE(registry.getNode(sink3Id).lock() == nullptr);
        REQUIRE(registry.getSink(sink3Id).lock() == nullptr);
        registry.removeSink(sink3Id);
        FORBID_CALL(outputMock, writeMessage(ANY(std::string)));
        
        // No olinkRelease call as there is no sink
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink3Id))));
        testedNode->unlinkRemote(sink3Id);

        testedNode.reset();
    }

    SECTION("link and unlink for all sinks that use the node, unlink also informs the sinks, that link is no longer in use")
    {
        registry.addSink(sink1);
        registry.addSink(sink2);

        // link sinks with one call
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink2Id))));
        testedNode->linkRemote(sink1Id);
        testedNode->linkRemote(sink2Id);
        // unlink sinks with one call
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink2->olinkObjectName()))));
        REQUIRE_CALL(*sink2, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode->unlinkRemote(sink2Id);

        testedNode.reset();
    }

    SECTION("sinks linked and unlinked many times - the state is not stored, the message is always sent")
    {
        registry.addSink(sink1);

        // link sinks
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);

        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);

        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);

        registry.removeSink(sink1Id);
        FORBID_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode.reset();
    }

    SECTION("invoking method and handling the invoke reply - successful scenario")
    {
        registry.addSink(sink1);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);
        registry.addSink(sink2);
        // Sink doesn't have to be linked to get back the response.
        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);
        const auto methodIdSink2 = ApiGear::ObjectLink::Name::createMemberId(sink2Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto firstRequestId = notSetRequestValue;
        auto secondRequestId = notSetRequestValue;

        // Invoke for sink2
        // Expect Invoke request to be sent on invokeRemote. Retrieve request id given by node.
        auto expectedrgumentsAsString = ApiGear::ObjectLink::getAsString(exampleArguments);
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink2, expectedrgumentsAsString }, *serializer)))
                    .LR_SIDE_EFFECT(firstRequestId = retrieveRequestId(_1));

        testedNode->invokeRemote(methodIdSink2, exampleArguments, [&outputMock](auto args) {outputMock.writeMessage(args.methodId + args.value.content.dump()); });
        REQUIRE(firstRequestId != notSetRequestValue);

        // Invoke for sink1
        // Expect Invoke request to be sent on invokeRemote. Retrieve request id given by node.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, expectedrgumentsAsString }, *serializer)))
            .LR_SIDE_EFFECT(secondRequestId = retrieveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + args.value.content.dump()); });
        REQUIRE(secondRequestId != notSetRequestValue);
        REQUIRE(secondRequestId != firstRequestId);

        // Handling reply

        // Send replies in different order than it was requested
        // First handler for sink1 will be called.
        // prepare reply
        auto functionResult2 = ContentSerializer::Value::serialize(17);
        auto writer1 = serializer->createWriter();
        const auto& invokeReplyMessage2 = ApiGear::ObjectLink::Protocol::invokeReplyMessage(*writer1, secondRequestId, methodIdSink1,  functionResult2 );
        // expect callback to be called
        auto result2 = ApiGear::ObjectLink::getAsString(functionResult2);
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink1 + result2));
        testedNode->handleMessage(serializer->toNetworkFormat(invokeReplyMessage2));

        // prepare reply
        auto functionResult1 = ContentSerializer::Value::serialize(74);
        auto writer2 = serializer->createWriter();
        const auto& invokeReplyMessage1 = ApiGear::ObjectLink::Protocol::invokeReplyMessage(*writer2, firstRequestId, methodIdSink2, functionResult1 );
        // expect callback to be called
        auto result1 = ApiGear::ObjectLink::getAsString(functionResult1);
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink2 + result1));
        testedNode->handleMessage(serializer->toNetworkFormat(invokeReplyMessage1));

        // test clean up for linked node
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);

        testedNode.reset();
    }

    SECTION("invoking method and handling the invoke reply - not matching object id scenario, it will be handled, the ")
    {
        registry.addSink(sink1);

        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto requestId = notSetRequestValue;

        // Prepare node to be waiting for invoke reply with requestId it creates on sending and for methodIdSink1.
        auto exampleArgumentsAsString = ApiGear::ObjectLink::getAsString(exampleArguments);
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, exampleArgumentsAsString }, *serializer)))
            .LR_SIDE_EFFECT(requestId = retrieveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + ApiGear::ObjectLink::getAsString(args.value)); });
        REQUIRE(requestId != notSetRequestValue);

        // Prepare reply with wrong request id.
        auto functionResult = ContentSerializer::Value::serialize(17);
        auto invokeReplyMessage = ApiGear::ObjectLink::Protocol::invokeReplyMessage(*(serializer->createWriter()), requestId, methodIdSink1, functionResult);
        // expect  no callback 
        auto result = ApiGear::ObjectLink::getAsString(functionResult);
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink1 + result));
        testedNode->handleMessage(serializer->toNetworkFormat(invokeReplyMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode.reset();
    }

    SECTION("handling invokeReply message - non matching requestId")
    {
        registry.addSink(sink1);
        registry.addSink(sink2);

        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);
        const auto methodIdSink2 = ApiGear::ObjectLink::Name::createMemberId(sink2Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto requestId = notSetRequestValue;
        auto otherRequestId = 157;

        // Prepare node to be waiting for invoke reply with requestId it creates on sending and for methodIdSink1.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, ApiGear::ObjectLink::getAsString(exampleArguments) }, *serializer)))
            .LR_SIDE_EFFECT(requestId = retrieveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments,
            [&outputMock](auto args){outputMock.writeMessage(args.methodId + ApiGear::ObjectLink::getAsString(args.value)); });
        REQUIRE(requestId != notSetRequestValue);
        REQUIRE(requestId != otherRequestId);

        // Prepare reply with wrong sink id in method id.
        auto functionResult = ContentSerializer::Value::serialize(17);
        auto invokeReplyMessage = ApiGear::ObjectLink::Protocol::invokeReplyMessage(*(serializer->createWriter()), otherRequestId, methodIdSink2, functionResult);
        // expect  no callback 
        FORBID_CALL(outputMock, writeMessage(ANY(std::string)));
        testedNode->handleMessage(serializer->toNetworkFormat(invokeReplyMessage));

        // test clean up
        testedNode.reset();
    }


    SECTION("handling signal message - successful scenario")
    {
        registry.addSink(sink1);
        registry.addSink(sink2);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink2Id))));
        testedNode->linkRemote(sink1Id);
        testedNode->linkRemote(sink2Id);

        auto signalId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, signalName);
        auto signalMessage = ApiGear::ObjectLink::Protocol::signalMessage(*(serializer->createWriter()), signalId, exampleArguments);

        REQUIRE_CALL(*sink2, olinkOnSignal(signalId, exampleArguments));
        testedNode->handleMessage(serializer->toNetworkFormat(signalMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink2->olinkObjectName()))));
        REQUIRE_CALL(*sink2, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode->unlinkRemote(sink2Id);

        testedNode.reset();
    }

    SECTION("handling signal message - no existing object")
    {
        registry.addSink(sink1);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);

        auto signalId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, signalName);
        auto signalMessage = ApiGear::ObjectLink::Protocol::signalMessage(*(serializer->createWriter()), signalId, exampleArguments);

        FORBID_CALL(*sink1, olinkOnSignal(signalId, exampleArguments));
        testedNode->handleMessage(serializer->toNetworkFormat(signalMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode.reset();
    }

    SECTION("handling init message - successful scenario")
    {
        registry.addSink(sink1);
        registry.addSink(sink2);
        // after sink is successfully linked the sink will get init message from server
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink2Id))));
        testedNode->linkRemote(sink1Id);
        testedNode->linkRemote(sink2Id);

        auto InitMessage = ApiGear::ObjectLink::Protocol::initMessage(*(serializer->createWriter()), sink2Id, exampleInitProperties);
        auto networkFormatedInit = serializer->toNetworkFormat(InitMessage);

        REQUIRE_CALL(*sink2, olinkOnInit(sink2Id, exampleInitProperties, testedNode.get()));
        testedNode->handleMessage(networkFormatedInit);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink2->olinkObjectName()))));
        REQUIRE_CALL(*sink2, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode->unlinkRemote(sink2Id);
        testedNode.reset();
    }

    SECTION("handling init message - for non existing object")
    {
        registry.addSink(sink1);

        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);

        // Send message for not registered sink 2.
        auto InitMessage = ApiGear::ObjectLink::Protocol::initMessage(*(serializer->createWriter()), sink2Id, exampleInitProperties);
        auto networkFormatedInit = serializer->toNetworkFormat(InitMessage);
        
        FORBID_CALL(*sink1, olinkOnInit(sink2Id, exampleInitProperties, testedNode.get()));
        testedNode->handleMessage(networkFormatedInit);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode.reset();
    }

    SECTION("changing property - successful scenario")
    {
        registry.addSink(sink1);
        registry.addSink(sink2);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink2Id))));
        testedNode->linkRemote(sink1Id);
        testedNode->linkRemote(sink2Id);

        const auto propertyId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, propertyName);
        auto requestPropertyChangeMessage = ApiGear::ObjectLink::Protocol::setPropertyMessage(*(serializer->createWriter()), propertyId, propertyValue);
        auto networkFormatedRequestPropertyChange = serializer->toNetworkFormat(requestPropertyChangeMessage);

        auto publishedPropertyChangeMessage = ApiGear::ObjectLink::Protocol::propertyChangeMessage(*(serializer->createWriter()), propertyId, otherPropertyValue);
        auto networkFormatedPublishedPropertyChange = serializer->toNetworkFormat(publishedPropertyChangeMessage);

        // Expect Request to be sent on setRemotePropertyCall.
        REQUIRE_CALL(outputMock, writeMessage(networkFormatedRequestPropertyChange));
        testedNode->setRemoteProperty(propertyId, propertyValue);

        // Expect handle property changed in proper sink after handling network message.
        REQUIRE_CALL(*sink2, olinkOnPropertyChanged(propertyId, otherPropertyValue));
        testedNode->handleMessage(networkFormatedPublishedPropertyChange);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink2->olinkObjectName()))));
        REQUIRE_CALL(*sink2, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode->unlinkRemote(sink2Id);
        testedNode.reset();
    }

    SECTION("handling propertyChanged message - for non existing object")
    {
        registry.addSink(sink1);
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::linkMessage(*(serializer->createWriter()), sink1Id))));
        testedNode->linkRemote(sink1Id);

        const auto propertyId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, propertyName);
        auto requestPropertyChangeMessage = ApiGear::ObjectLink::Protocol::setPropertyMessage(*(serializer->createWriter()), propertyId, propertyValue);
        auto networkFormatedRequestPropertyChange = serializer->toNetworkFormat(requestPropertyChangeMessage);

        auto publishedPropertyChangeMessage = ApiGear::ObjectLink::Protocol::propertyChangeMessage(*(serializer->createWriter()), propertyId, otherPropertyValue);
        auto networkFormatedPublishedPropertyChange = serializer->toNetworkFormat(publishedPropertyChangeMessage);

        // Expect Request to be sent on setRemotePropertyCall, even if it is not correct - this node doesn't serve sink2.
        REQUIRE_CALL(outputMock, writeMessage(networkFormatedRequestPropertyChange));
        testedNode->setRemoteProperty(propertyId, propertyValue);

        // No call of handle message.
        FORBID_CALL(*sink1, olinkOnPropertyChanged(propertyId, otherPropertyValue));
        testedNode->handleMessage(networkFormatedPublishedPropertyChange);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(serializer->toNetworkFormat(ApiGear::ObjectLink::Protocol::unlinkMessage(*(serializer->createWriter()), sink1->olinkObjectName()))));
        REQUIRE_CALL(*sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);
        testedNode.reset();
    }

    SECTION("Messages are not sent if the write function is not set.")
    {
        // keep as ptr to destroy before going out of scope of test. The mock does not do well with expectations left in tests for test tear down.
        auto nodeWithoutSetWriteFunction = ApiGear::ObjectLink::ClientNode::create(registry);
        
        nodeWithoutSetWriteFunction->onLog([&outputMock](auto level, const auto& msg){outputMock.logMessage(level, msg); });
        nodeWithoutSetWriteFunction->onWrite(nullptr, serializer);
        nodeWithoutSetWriteFunction->setLogLevel(ApiGear::ObjectLink::LogLevel::Info);

        registry.addSink(sink1);
        registry.addSink(sink2);

        ALLOW_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Debug, ANY(std::string)));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({".link", sink1Id})));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "Messages are not sent if the write function is not set"));

        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({sink2Id, ".link" })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "Messages are not sent if the write function is not set"));
        nodeWithoutSetWriteFunction->linkRemote(sink1Id);
        nodeWithoutSetWriteFunction->linkRemote(sink2Id);

        // Expectations on dtor
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({ "unlink", sink1Id })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "Messages are not sent if the write function is not set"));

        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({ sink2Id, "unlink" })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "Messages are not sent if the write function is not set"));

        REQUIRE_CALL(*sink1, olinkOnRelease());
        REQUIRE_CALL(*sink2, olinkOnRelease());
        nodeWithoutSetWriteFunction->unlinkRemote(sink1Id);
        nodeWithoutSetWriteFunction->unlinkRemote(sink2Id);
        nodeWithoutSetWriteFunction.reset();
    }
}