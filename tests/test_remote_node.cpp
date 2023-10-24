
#include <catch2/catch.hpp>

#include "olink/iobjectsource.h"
#include "olink/remotenode.h"
#include "olink/remoteregistry.h"
#include "olink/core/types.h"

#include "mocks.h"
#include "matchers.h"

namespace {
    // helper constants used in test.
    std::string source1Id = "tests.source1";
    std::string source2Id = "tests.source2";
    std::string propertyName = "exampleProprety";
    std::string stringProperty = "some string";
    std::string methodName = "exampleMethod";
    std::string signalName = "exampleSingal";
    auto propertyValue = ApiGear::ObjectLink::propertyToContent(8);
    int someIntValue = 9;
    bool someBoolValue = false;
    auto otherPropertyValue = ApiGear::ObjectLink::propertyToContent(115);
    auto exampleInitProperties = ApiGear::ObjectLink::argumentsToContent(ApiGear::ObjectLink::toInitialProperty(propertyName, stringProperty),
                                                                         ApiGear::ObjectLink::toInitialProperty(std::string("property2"), someIntValue),
                                                                         ApiGear::ObjectLink::toInitialProperty(std::string("arg2"), someBoolValue));
    auto exampleArguments = ApiGear::ObjectLink::argumentsToContent(std::string("some_string"), 9, false);

    // Converter used in tests, should be same as one used by node.
    ApiGear::ObjectLink::MessageConverter converter(ApiGear::ObjectLink::MessageFormat::JSON);
}

TEST_CASE("Remote Node")
{
    auto linkMessageSource1 = converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(source1Id));
    auto linkMessageSource2 = converter.toString(ApiGear::ObjectLink::Protocol::linkMessage(source2Id));
    auto unlinkMessageSource1 = converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(source1Id));
    auto unlinkMessageSource2 = converter.toString(ApiGear::ObjectLink::Protocol::unlinkMessage(source2Id));

    // Objects required for most tests

    // Sink mocks that always return associated with them ids.
    auto source1 = std::make_shared<SourceObjectMock>();
    auto source2 = std::make_shared<SourceObjectMock>();

    ALLOW_CALL(*source1, olinkObjectName()).RETURN(source1Id);
    ALLOW_CALL(*source2, olinkObjectName()).RETURN(source2Id);
    
    ApiGear::ObjectLink::RemoteRegistry registry;
    
    // Output mock to monitor written messages.
    OutputMock outputMock;


    // Test node kept as ptr to destroy before going out of scope of test. Allows testing destructor.
    auto testedNode = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
    // Setting up an output for sending messages. This function must be set by user.
    testedNode->onWrite([&outputMock](const auto& msg){outputMock.writeMessage(msg); });

    SECTION("Typical setup and tear down scenario - the node ends life before the source")
    {
        registry.addSource(source1);
        registry.addSource(source2);

        REQUIRE(registry.getNodes(source1Id).size() == 0);
        REQUIRE(registry.getNodes(source2Id).size() == 0);

        REQUIRE_CALL(*source1, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(*source1, olinkLinked(source1Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(*source2, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(*source2, olinkLinked(source2Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source1Id, exampleInitProperties))));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source2Id, exampleInitProperties))));
        testedNode->handleMessage(linkMessageSource1);
        testedNode->handleMessage(linkMessageSource2);

        REQUIRE(registry.getNodes(source1Id).size() == 1);
        REQUIRE(registry.getNodes(source2Id).size() == 1);


        REQUIRE_CALL(*source1, olinkUnlinked(source1Id));
        REQUIRE_CALL(*source2, olinkUnlinked(source2Id));
        testedNode->handleMessage(unlinkMessageSource1);
        testedNode->handleMessage(unlinkMessageSource2);

        REQUIRE(registry.getNodes(source1Id).size() == 0);
        REQUIRE(registry.getNodes(source2Id).size() == 0);
        
        testedNode.reset();
    }

    SECTION("Source must be added to registry before linking, link requests are not stored.")
    {

        testedNode->handleMessage(linkMessageSource1);
        
        // No nodes added.
        registry.addSource(source1);
        REQUIRE(registry.getNodes(source1Id).size() == 0);

        // Second linking works
        REQUIRE_CALL(*source1, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(*source1, olinkLinked(source1Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source1Id, exampleInitProperties))));
        testedNode->handleMessage(linkMessageSource1);
        REQUIRE(registry.getNodes(source1Id).size() == 1);

        testedNode.reset();
    }


    SECTION("if not on unlink, but node dies anyway, node removes itself from registry")
    {
        registry.addSource(source1);
        REQUIRE_CALL(*source1, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(*source1, olinkLinked(source1Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source1Id, exampleInitProperties))));
        testedNode->handleMessage(linkMessageSource1);
        REQUIRE(registry.getNodes(source1Id).size() == 1);

        testedNode.reset();

        REQUIRE(registry.getNodes(source1Id).size() == 0);
    }

    SECTION("handling request property change")
    {
        registry.addSource(source1);
        REQUIRE_CALL(*source1, olinkLinked(source1Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(*source1, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source1Id, exampleInitProperties))));
        testedNode->handleMessage(linkMessageSource1);
        REQUIRE(registry.getNodes(source1Id).size() == 1);

        auto propertyId1 = ApiGear::ObjectLink::Name::createMemberId(source1Id, propertyName);
        auto propertyId2 = ApiGear::ObjectLink::Name::createMemberId(source2Id, propertyName);

        auto propertyChangeRequest1 = converter.toString(ApiGear::ObjectLink::Protocol::setPropertyMessage(propertyId1, {propertyValue}));
        auto propertyChangeRequest2 = converter.toString(ApiGear::ObjectLink::Protocol::setPropertyMessage(propertyId2, {propertyValue}));


        REQUIRE_CALL(*source1, olinkSetProperty(propertyId1, propertyValue));
        testedNode->handleMessage(propertyChangeRequest1);
        // Won't have any effect, no source 2 added
        testedNode->handleMessage(propertyChangeRequest2);
    }

    SECTION("invoking method and handling the invoke reply - successful scenario")
    {

        registry.addSource(source1);
        REQUIRE_CALL(*source1, olinkLinked(source1Id, ANY(ApiGear::ObjectLink::IRemoteNode*)));
        REQUIRE_CALL(*source1, olinkCollectProperties()).RETURN(exampleInitProperties);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::initMessage(source1Id, exampleInitProperties))));
        testedNode->handleMessage(linkMessageSource1);
        REQUIRE(registry.getNodes(source1Id).size() == 1);

        auto methodId1 = ApiGear::ObjectLink::Name::createMemberId(source1Id, methodName);
        auto methodId2 = ApiGear::ObjectLink::Name::createMemberId(source2Id, methodName);

        int requestId1 = 189;
        int requestId2 = 32;

        auto invokeMethod1 = converter.toString(ApiGear::ObjectLink::Protocol::invokeMessage(requestId1, methodId1, exampleArguments));
        auto invokeMethod2 = converter.toString(ApiGear::ObjectLink::Protocol::invokeMessage(requestId2, methodId2, exampleArguments));

        auto result = ApiGear::ObjectLink::invokeReturnValue(123);
        REQUIRE_CALL(*source1, olinkInvoke(methodId1, exampleArguments)).RETURN(result);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::invokeReplyMessage(requestId1, methodId1, { result }))));
        testedNode->handleMessage(invokeMethod1);
        // Won't have any effect, no source 2 added
        testedNode->handleMessage(invokeMethod2);
       
        testedNode.reset();
    }


    SECTION("sending signal message")
    {
        auto signalId = ApiGear::ObjectLink::Name::createMemberId(source1Id, signalName);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::signalMessage(signalId, exampleArguments))));
        testedNode->notifySignal(signalId, exampleArguments);
    }

    SECTION("sending property")
    {
        auto propertyId = ApiGear::ObjectLink::Name::createMemberId(source1Id, propertyName);
        REQUIRE_CALL(outputMock, writeMessage(converter.toString(ApiGear::ObjectLink::Protocol::propertyChangeMessage(propertyId, { propertyValue }))));
        testedNode->notifyPropertyChange(propertyId, propertyValue);
    }

    SECTION("Messages are not sent if the write function is not set.")
    {
        // keep as ptr to destroy before going out of scope of test. The mock does not do well with expectations left in tests for test tear down.
        auto nodeWithoutSetWriteFunction = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);

        nodeWithoutSetWriteFunction->onLog([&outputMock](auto level, const auto& msg){outputMock.logMessage(level, msg); });
        nodeWithoutSetWriteFunction->setLogLevel(ApiGear::ObjectLink::LogLevel::Info);

        auto signalId = ApiGear::ObjectLink::Name::createMemberId(source1Id, "any");
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Debug, contains_keywords({ signalId })));
        REQUIRE_CALL(outputMock, logMessage(ApiGear::ObjectLink::LogLevel::Warning, "Messages are not sent if the write function is not set"));
        nodeWithoutSetWriteFunction->notifySignal(signalId, {});

        nodeWithoutSetWriteFunction.reset();
    }
}