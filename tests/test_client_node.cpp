
#include <catch2/catch.hpp>

#include "olink/iobjectsink.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"
#include "olink/core/types.h"

#include "mocks.h"
#include "matchers.h"

namespace {
    // helper constants used in test.
    std::string sink1Id = "tests.sink1";
    std::string sink2Id = "tests.sink2";
    std::string propertyName = "exampleProprety";
    nlohmann::json propertyValue = {{8}};
    nlohmann::json otherPropertyValue = {{115}};
    nlohmann::json exampleInitProperties = { {propertyName, "some_string" }, {"property2",  9}, {"arg2", false } };
    std::string methodName = "exampleMethod";
    std::string signalName = "exampleSingal";
    nlohmann::json exampleArguments = {{"arg1", "some_string" }, {"arg2",  9}, {"arg2", false } };

    // Converter used in tests, should be same as one used by node.
    ApiGear::ObjectLink::MessageConverter converter(ApiGear::ObjectLink::MessageFormat::JSON);

    // Helper function to add sink to registry, and set a node for it.
    void registerSinkWithNode(ApiGear::ObjectLink::ClientRegistry& registry, ApiGear::ObjectLink::IObjectSink& sink, ApiGear::ObjectLink::ClientNode& clientNode)
    {
        registry.addSink(sink);
        registry.setNode(clientNode, sink.olinkObjectName());
    }
    // Helper function which gets from a InvokeMessage a requestId given by clientNode.
    int retriveRequestId(const std::string& networkMessage)
    {
        const auto& requestMessage = converter.fromString(networkMessage);
        REQUIRE(requestMessage[0].get<int>() == static_cast<int>(ApiGear::ObjectLink::MsgType::Invoke));
        int result = requestMessage[1].get<int>();
        return result;
    };

}

TEST_CASE("Client Node")
{

    // Objects required for most tests
    
    // Sink mocks that always return associated with them ids.
    SinkObjectMock sink1, sink2;
    ALLOW_CALL(sink1, olinkObjectName()).RETURN(sink1Id);
    ALLOW_CALL(sink2, olinkObjectName()).RETURN(sink2Id);
    // Output mock to monitor written messages.
    OutputMock outputMock;

    ApiGear::ObjectLink::ClientRegistry registry;

    // Test node kept as ptr to destroy before going out of scope of test. Allows testing destructor.
    auto testedNode = std::make_unique<ApiGear::ObjectLink::ClientNode>(registry);
    // Setting up an output for sending messages. This function must be set by user.
    testedNode->onWrite([&outputMock](const auto& msg){outputMock.writeMessage(msg); });

    SECTION("Typical setup and tear down scenario - the node ends life before the sinks, client node unlinks sinks automatically")
    {
        registerSinkWithNode(registry, sink2, *testedNode);
        registerSinkWithNode(registry, sink1, *testedNode);

        // link sinks
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink2Id))));
        testedNode->linkRemoteForAllSinks();

        // Expectations on dtor
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2Id))));

        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();

        REQUIRE(registry.getNode(sink1Id) == nullptr);
        REQUIRE(registry.getNode(sink2Id) == nullptr);
    }

    SECTION("Wrong scenario sinks is deleted, before removing from registry and unlinking")
    {
        auto sink3 = std::make_unique<SinkObjectMock>();
        std::string sink3Id = "tests.sink3";
        ALLOW_CALL(*sink3, olinkObjectName()).RETURN(sink3Id);

        registerSinkWithNode(registry, *sink3, *testedNode);

        sink3.reset();
        // Still needs to be removed from registry, the resource is hanging, deleting now the node would crash
        // node would like to send unlink message
        REQUIRE(registry.getNode(sink3Id) != nullptr);
        // Unlink may never be called - registry gives invalid sink object.
        // Without removal of the object node will crash
        registry.removeSink(sink3Id);
        FORBID_CALL(outputMock, writeMessage(ANY(std::string)));
        
        testedNode.reset();
    }
   
    SECTION("Before sink removal from registry user has to unlink sink manually")
    {
        auto sink3 = std::make_unique<SinkObjectMock>();
        std::string sink3Id = "tests.sink3";
        ALLOW_CALL(*sink3, olinkObjectName()).RETURN(sink3Id);

        registerSinkWithNode(registry, *sink3, *testedNode);

        // unlink message may be send regardless if the linking was done or not
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink3->olinkObjectName()))));
        REQUIRE_CALL(*sink3, olinkOnRelease());
        testedNode->unlinkRemote(sink3Id);

        registry.removeSink(sink3Id);
        REQUIRE(registry.getNode(sink3Id) == nullptr);

        testedNode.reset();
    }


    SECTION("link and unlink for all sinks that use the node, unlink also informs the sinks, that link is no longer in use")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        // link sinks with one call
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink2Id))));
        testedNode->linkRemoteForAllSinks();
        // unlink sinks with one call
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode->unlinkRemoteForAllSinks();

        // Expectations on node dtor - node wasn't removed in registry, nor were the sinks
        // cleaning up resources makes second round of unlinking.
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("sinks linked and unlinked many times - the state is not stored, the message is always sent")
    {
        registerSinkWithNode(registry, sink1, *testedNode);

        // link sinks
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        testedNode->linkRemote(sink1Id);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        testedNode->linkRemote(sink1Id);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        testedNode->linkRemoteForAllSinks();

        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);

        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode->unlinkRemote(sink1Id);

        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode->unlinkRemoteForAllSinks();

        registry.removeSink(sink1Id);
        FORBID_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1Id))));
        testedNode.reset();
    }

    SECTION("invoking method and handling the invoke reply - successful scenario")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);
        const auto methodIdSink2 = ApiGear::ObjectLink::Name::createMemberId(sink2Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto firstRequestId = notSetRequestValue;
        auto secondRequestId = notSetRequestValue;

        // Invoke for sink2
        // Expect Invoke request to be sent on invokeRemote. Retrieve request id given by node.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink2, exampleArguments.dump()}, converter)))
                    .LR_SIDE_EFFECT(firstRequestId = retriveRequestId(_1));
        testedNode->invokeRemote(methodIdSink2, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + args.value.dump()); });
        REQUIRE(firstRequestId != notSetRequestValue);

        // Invoke for sink1
        // Expect Invoke request to be sent on invokeRemote. Retrieve request id given by node.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, exampleArguments.dump() }, converter)))
            .LR_SIDE_EFFECT(secondRequestId = retriveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + args.value.dump()); });
        REQUIRE(secondRequestId != notSetRequestValue);
        REQUIRE(secondRequestId != firstRequestId);

        // Handling reply

        // Send replies in different order than it was requested
        // First handler for sink1 will be called.
        // prepare reply
        nlohmann::json functionResult2 = {{ 17 } };
        const auto& invokeReplyMessage2 = ApiGear::ObjectLink::Protocol::invokeReplyMessage(secondRequestId, methodIdSink1, functionResult2);
        // expect callback to be called
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink1 + functionResult2.dump()));
        testedNode->handleMessage(converter.toString(invokeReplyMessage2));

        // prepare reply
        nlohmann::json functionResult1 = {{ 74 } };
        const auto& invokeReplyMessage1 = ApiGear::ObjectLink::Protocol::invokeReplyMessage(firstRequestId, methodIdSink2, functionResult1);
        // expect callback to be called
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink2 + functionResult1.dump()));
        testedNode->handleMessage(converter.toString(invokeReplyMessage1));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("invoking method and handling the invoke reply - not matching object id scenario, it will be handled, the ")
    {
        registerSinkWithNode(registry, sink1, *testedNode);

        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto requestId = notSetRequestValue;

        // Prepare node to be waiting for invoke reply with requestId it creates on sending and for methodIdSink1.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, exampleArguments.dump() }, converter)))
            .LR_SIDE_EFFECT(requestId = retriveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + args.value.dump()); });
        REQUIRE(requestId != notSetRequestValue);

        // Prepare reply with wrong request id.
        nlohmann::json functionResult = { { 17 } };
        const auto& invokeReplyMessage = ApiGear::ObjectLink::Protocol::invokeReplyMessage(requestId, methodIdSink1, functionResult);
        // expect  no callback 
        REQUIRE_CALL(outputMock, writeMessage(methodIdSink1 + functionResult.dump()));
        testedNode->handleMessage(converter.toString(invokeReplyMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("handling invokeReply message - non matching requestId")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        const auto methodIdSink1 = ApiGear::ObjectLink::Name::createMemberId(sink1Id, methodName);
        const auto methodIdSink2 = ApiGear::ObjectLink::Name::createMemberId(sink2Id, methodName);

        // Init to some value we know for sure the first requestId won't have;
        auto notSetRequestValue = 999999999;
        auto requestId = notSetRequestValue;
        auto otherRequestId = 157;

        // Prepare node to be waiting for invoke reply with requestId it creates on sending and for methodIdSink1.
        REQUIRE_CALL(outputMock, writeMessage(network_message_contains_keywords({ methodIdSink1, exampleArguments.dump() }, converter)))
            .LR_SIDE_EFFECT(requestId = retriveRequestId(_1));
        testedNode->invokeRemote(methodIdSink1, exampleArguments, [&outputMock](auto args){outputMock.writeMessage(args.methodId + args.value.dump()); });
        REQUIRE(requestId != notSetRequestValue);
        REQUIRE(requestId != otherRequestId);

        // Prepare reply with wrong sink id in method id.
        nlohmann::json functionResult = { { 17 } };
        const auto& invokeReplyMessage = ApiGear::ObjectLink::Protocol::invokeReplyMessage(otherRequestId, methodIdSink2, functionResult);
        // expect  no callback 
        FORBID_CALL(outputMock, writeMessage(ANY(std::string)));
        testedNode->handleMessage(converter.toString(invokeReplyMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }


    SECTION("handling signal message - successful scenario")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        auto signalId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, signalName);
        const auto& signalMessage = ApiGear::ObjectLink::Protocol::signalMessage(signalId, exampleArguments);

        REQUIRE_CALL(sink2, olinkOnSignal(signalId, exampleArguments));
        testedNode->handleMessage(converter.toString(signalMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("handling signal message - no existing object")
    {
        registerSinkWithNode(registry, sink1, *testedNode);

        auto signalId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, signalName);
        const auto& signalMessage = ApiGear::ObjectLink::Protocol::signalMessage(signalId, exampleArguments);

        FORBID_CALL(sink1, olinkOnSignal(signalId, exampleArguments));
        testedNode->handleMessage(converter.toString(signalMessage));

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("handling init message - successful scenario")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        // after sink is successfully linked the sink will get init message from server
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink2Id))));
        testedNode->linkRemoteForAllSinks();

        const auto& InitMessage = ApiGear::ObjectLink::Protocol::initMessage(sink2Id, exampleInitProperties);
        const auto& networkFormatedInit = converter.toString(InitMessage);

        REQUIRE_CALL(sink2, olinkOnInit(sink2Id, exampleInitProperties, testedNode.get()));
        testedNode->handleMessage(networkFormatedInit);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("handling init message - for non existing object")
    {
        registerSinkWithNode(registry, sink1, *testedNode);

        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(sink1Id))));
        testedNode->linkRemote(sink1Id);

        // Send message for not registered sink 2.
        const auto& InitMessage = ApiGear::ObjectLink::Protocol::initMessage(sink2Id, exampleInitProperties);
        const auto& networkFormatedInit = converter.toString(InitMessage);
        
        FORBID_CALL(sink1, olinkOnInit(sink2Id, exampleInitProperties, testedNode.get()));
        testedNode->handleMessage(networkFormatedInit);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("changing property - successful scenario")
    {
        registerSinkWithNode(registry, sink1, *testedNode);
        registerSinkWithNode(registry, sink2, *testedNode);

        const auto propertyId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, propertyName);
        const auto& requestPropertyChangeMessage = ApiGear::ObjectLink::Protocol::setPropertyMessage(propertyId, propertyValue);
        const auto& networkFormatedRequestPropertyChange = converter.toString(requestPropertyChangeMessage);

        const auto& publishedPropertyChangeMessage = ApiGear::ObjectLink::Protocol::propertyChangeMessage(propertyId, otherPropertyValue);
        const auto& networkFormatedPublishedPropertyChange = converter.toString(publishedPropertyChangeMessage);

        // Expect Request to be sent on setRemotePropertyCall.
        REQUIRE_CALL(outputMock, writeMessage(networkFormatedRequestPropertyChange));
        testedNode->setRemoteProperty(propertyId, propertyValue);

        // Expect handle property changed in proper sink after handling network message.
        REQUIRE_CALL(sink2, olinkOnPropertyChanged(propertyId, otherPropertyValue));
        testedNode->handleMessage(networkFormatedPublishedPropertyChange);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink2.olinkObjectName()))));
        REQUIRE_CALL(sink2, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("handling propertyChanged message - for non existing object")
    {
        registerSinkWithNode(registry, sink1, *testedNode);

        const auto propertyId = ApiGear::ObjectLink::Name::createMemberId(sink2Id, propertyName);
        const auto& requestPropertyChangeMessage = ApiGear::ObjectLink::Protocol::setPropertyMessage(propertyId, propertyValue);
        const auto& networkFormatedRequestPropertyChange = converter.toString(requestPropertyChangeMessage);

        const auto& publishedPropertyChangeMessage = ApiGear::ObjectLink::Protocol::propertyChangeMessage(propertyId, otherPropertyValue);
        const auto& networkFormatedPublishedPropertyChange = converter.toString(publishedPropertyChangeMessage);

        // Expect Request to be sent on setRemotePropertyCall, even if it is not correct - this node doesn't serve sink2.
        REQUIRE_CALL(outputMock, writeMessage(networkFormatedRequestPropertyChange));
        testedNode->setRemoteProperty(propertyId, propertyValue);

        // No call of handle message.
        FORBID_CALL(sink1, olinkOnPropertyChanged(propertyId, otherPropertyValue));
        testedNode->handleMessage(networkFormatedPublishedPropertyChange);

        // test clean up
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(sink1.olinkObjectName()))));
        REQUIRE_CALL(sink1, olinkOnRelease());
        testedNode.reset();
    }

    SECTION("Messages are not sent if the write function is not set.")
    {
        OutputMock outputMock;
        ApiGear::ObjectLink::ClientRegistry registry;
        // keep as ptr to destroy before going out of scope of test. The mock does not do well with expectations left in tests for test tear down.
        auto nodeWithoutSetWriteFunction = std::make_unique<ApiGear::ObjectLink::ClientNode>(registry);
        
        nodeWithoutSetWriteFunction->onLog([&outputMock](auto level, const auto& msg){outputMock.logMessage(level, msg); });

        registerSinkWithNode(registry, sink1, *nodeWithoutSetWriteFunction);
        registerSinkWithNode(registry, sink2, *nodeWithoutSetWriteFunction);

        ALLOW_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Debug, ANY(std::string)));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({".link", sink1Id})));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "no writer set, can not write"));

        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({sink2Id, ".link" })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "no writer set, can not write"));

        nodeWithoutSetWriteFunction->linkRemoteForAllSinks();

        // Expectations on dtor
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({ "unlink", sink1Id })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "no writer set, can not write"));

        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Info, contains_keywords({ sink2Id, "unlink" })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "no writer set, can not write"));

        REQUIRE_CALL(sink1, olinkOnRelease());
        REQUIRE_CALL(sink2, olinkOnRelease());
        nodeWithoutSetWriteFunction.reset();
    }
}