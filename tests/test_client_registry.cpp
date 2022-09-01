
#include <catch2/catch.hpp>
#include "mocks.h"

#include "olink/iobjectsink.h"
#include "olink/clientnode.h"
#include "olink/clientregistry.h"


TEST_CASE("client registry")
{
    // Create two sink MOCK objects, that always return associated with them ids.
    SinkObjectMock sink1, sink2;
    std::string sink1Id= "tests.sink1";
    std::string sink2Id= "tests.sink2";
    ALLOW_CALL(sink1, olinkObjectName()).RETURN(sink1Id);
    ALLOW_CALL(sink2, olinkObjectName()).RETURN(sink2Id);
    // Real node objects are necessary for the Client Registry
    // Node class in its destructor informs sink registerd for it in registry that it was released.
    ALLOW_CALL(sink1, olinkOnRelease());
    ALLOW_CALL(sink2, olinkOnRelease());


    SECTION("Add object and set node for it") {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry);
        clientRegistry.addObject(sink1);

        std::vector<std::string> empty = { };
        REQUIRE(clientRegistry.getObjectsId(node1) == empty);
        
        clientRegistry.setNode(node1, sink1Id);
        std::vector<std::string> expectedId = { sink1Id };
        REQUIRE(clientRegistry.getObjectsId(node1) == expectedId);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);
     }

    SECTION("Sink can be added after setting node")
    {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry);
        clientRegistry.addObject(sink1);

        REQUIRE(clientRegistry.getObjectsId(node1).size() == 0);
        clientRegistry.setNode(node1, sink1Id);
        std::vector<std::string> expectedId = { sink1Id };
        REQUIRE(clientRegistry.getObjectsId(node1) == expectedId);

        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);
    }

    SECTION("Node can be set many sinks")
    {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry), node2(clientRegistry);
        clientRegistry.addObject(sink1);
        clientRegistry.addObject(sink2);

        REQUIRE(clientRegistry.getObjectsId(node1).size() == 0);
        clientRegistry.setNode(node1, sink1Id);
        clientRegistry.setNode(node1, sink2Id);
        std::string notExisitnigSinkId = "notExisitnigSinkId";
        clientRegistry.setNode(node1, notExisitnigSinkId);
        // No id for not existing sink, although it is added in registry.
        std::vector<std::string> expectedIds = { sink1Id, sink2Id };
        REQUIRE_THAT(clientRegistry.getObjectsId(node1), Catch::Matchers::UnorderedEquals(expectedIds));

        // For all id we're getting same node
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);
        REQUIRE(clientRegistry.getClientNode(sink2Id) == &node1);
        REQUIRE(clientRegistry.getClientNode(notExisitnigSinkId) == &node1);
        // For all id's we're getting added objects
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);
        REQUIRE(clientRegistry.getObjectSink(sink2Id) == &sink2);
        REQUIRE(clientRegistry.getObjectSink(notExisitnigSinkId) == nullptr);
    }

    SECTION("Node can be changed for object only after unset.")
    {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry), node2(clientRegistry);
        clientRegistry.addObject(sink1);

        clientRegistry.setNode(node1, sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);

        clientRegistry.setNode(node2, sink1Id);
        // Still node1 set for id
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);

        clientRegistry.unsetNode(node1, sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == nullptr);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);

        clientRegistry.setNode(node2, sink1Id);
        // Now node2 is set
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node2);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);
    }

    SECTION("Object can be registered for same id only after removing older object with its node.")
    {
        SinkObjectMock differentSinkForId1;
        ALLOW_CALL(differentSinkForId1, olinkOnRelease());
        ALLOW_CALL(differentSinkForId1, olinkObjectName()).RETURN(sink1Id);

        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry);
        clientRegistry.addObject(sink1);
        clientRegistry.setNode(node1, sink1Id);

        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);

        clientRegistry.addObject(differentSinkForId1);
        // Still sink1Id registerd with a node1 for id
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &sink1);

        clientRegistry.removeObject(sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == nullptr);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == nullptr);

        clientRegistry.addObject(differentSinkForId1);

        // Now differentSinkForId1 is set for id, but no node is assigned.
        REQUIRE(clientRegistry.getClientNode(sink1Id) == nullptr);
        REQUIRE(clientRegistry.getObjectSink(sink1Id) == &differentSinkForId1);
    }

    SECTION("To unset node both, the node and id must match.")
    {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        ApiGear::ObjectLink::ClientNode node1(clientRegistry), node2(clientRegistry);
        clientRegistry.addObject(sink1);

        clientRegistry.setNode(node1, sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);

        clientRegistry.unsetNode(node2, sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);

        clientRegistry.unsetNode(node1, sink2Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == &node1);

        clientRegistry.unsetNode(node1, sink1Id);
        REQUIRE(clientRegistry.getClientNode(sink1Id) == nullptr);
    }

    SECTION("Removing unexisting object doesn't crash")
    {
        ApiGear::ObjectLink::ClientRegistry clientRegistry;
        clientRegistry.addObject(sink1);

        clientRegistry.removeObject("some otherId");
    }
}