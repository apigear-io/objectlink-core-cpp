
#include <catch2/catch.hpp>
#include "mocks.h"

#include "olink/iobjectsink.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"


TEST_CASE("client registry")
{
    // Sink MOCK objects, that always return associated with them ids.
    auto sink1 = std::make_shared<SinkObjectMock>();
    auto sink2 = std::make_shared<SinkObjectMock>();
    std::string sink1Id= "tests.sink1";
    std::string sink2Id= "tests.sink2";
    ALLOW_CALL(*sink1, olinkObjectName()).RETURN(sink1Id);
    ALLOW_CALL(*sink2, olinkObjectName()).RETURN(sink2Id);
    // Real node objects are necessary for the Client Registry
    // Node class in its destructor informs sink registered for it in registry that it was released.
    ALLOW_CALL(*sink1, olinkOnRelease());
    ALLOW_CALL(*sink2, olinkOnRelease());

    ApiGear::ObjectLink::ClientRegistry clientRegistry;
    clientRegistry.addSink(sink1);

    SECTION("Add object and set node for it") {

        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);

        std::vector<std::string> empty = { };
        auto id = node1->getNodeId();
        clientRegistry.setNode(id, sink1Id);
        std::vector<std::string> expectedId = { sink1Id };
        REQUIRE(clientRegistry.getObjectIds(id) == expectedId);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);
     }

    SECTION("Sink can be added after setting node")
    {
        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);

        auto id = node1->getNodeId();
        clientRegistry.setNode(id, sink1Id);
        std::vector<std::string> expectedId = { sink1Id };
        REQUIRE(clientRegistry.getObjectIds(id) == expectedId);

        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);
    }

    SECTION("Node can be set many sinks")
    {
        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        auto node2 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        clientRegistry.addSink(sink2);

        auto nodeId = node1->getNodeId();
        clientRegistry.setNode(nodeId, sink1Id);
        clientRegistry.setNode(nodeId, sink2Id);
        std::string notExisitnigSinkId = "notExisitnigSinkId";
        clientRegistry.setNode(nodeId, notExisitnigSinkId);
        std::vector<std::string> expectedIds = { notExisitnigSinkId, sink1Id, sink2Id };
        REQUIRE_THAT(clientRegistry.getObjectIds(nodeId), Catch::Matchers::UnorderedEquals(expectedIds));

        // For all ids we're getting same node
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);
        REQUIRE(clientRegistry.getNode(sink2Id).lock() == node1);
        REQUIRE(clientRegistry.getNode(notExisitnigSinkId).lock() == node1);
        // For all ids we're getting added objects
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);
        REQUIRE(clientRegistry.getSink(sink2Id).lock() == sink2);
        REQUIRE(clientRegistry.getSink(notExisitnigSinkId).lock() == nullptr);
    }

    SECTION("Node can be changed for object only after unset.")
    {
        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        auto node2 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        auto nodeId1 = node1->getNodeId();
        auto nodeId2 = node2->getNodeId();

        clientRegistry.setNode(nodeId1, sink1Id);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);

        clientRegistry.setNode(nodeId2, sink1Id);
        // Still node1 set for id
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);

        clientRegistry.unsetNode(sink1Id);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == nullptr);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);

        clientRegistry.setNode(nodeId2, sink1Id);
        // Now node2 is set
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node2);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);
    }

    SECTION("Object can be registered for same id only after removing older object with its node.")
    {
        auto differentSinkForId1 = std::make_shared<SinkObjectMock>();
        ALLOW_CALL(*differentSinkForId1, olinkOnRelease());
        ALLOW_CALL(*differentSinkForId1, olinkObjectName()).RETURN(sink1Id);

        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        auto nodeId1 = node1->getNodeId();

        clientRegistry.setNode(nodeId1, sink1Id);

        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);

        clientRegistry.addSink(differentSinkForId1);
        // Still sink1Id registered with a node1 for id
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == node1);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == sink1);

        clientRegistry.removeSink(sink1Id);
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == nullptr);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == nullptr);

        clientRegistry.addSink(differentSinkForId1);

        // Now differentSinkForId1 is set for id, but no node is assigned.
        REQUIRE(clientRegistry.getNode(sink1Id).lock() == nullptr);
        REQUIRE(clientRegistry.getSink(sink1Id).lock() == differentSinkForId1);
    }

    SECTION("Add node first, then the sink")
    {
        auto node1 = ApiGear::ObjectLink::ClientNode::create(clientRegistry);
        auto nodeId1 = node1->getNodeId();
        clientRegistry.setNode(nodeId1, sink2Id);
        clientRegistry.addSink(sink2);

        REQUIRE(clientRegistry.getSink(sink2Id).lock() == sink2);
    }


    SECTION("Removing not existing object doesn't crash")
    {
        clientRegistry.removeSink("some otherId");
    }
}