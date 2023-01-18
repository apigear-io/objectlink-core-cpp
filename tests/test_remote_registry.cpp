
#include <catch2/catch.hpp>
#include "mocks.h"

#include "olink/iobjectsource.h"
#include "olink/remotenode.h"
#include "olink/remoteregistry.h"


TEST_CASE("server registry simple tests without threads")
{
    // Source MOCK objects, that always return associated with them ids.
    auto source1 = std::make_shared<SourceObjectMock>();
    auto source2 = std::make_shared<SourceObjectMock>();
    std::string source1Id = "tests.source1";
    std::string source2Id = "tests.source2";
    ALLOW_CALL(*source1, olinkObjectName()).RETURN(source1Id);
    ALLOW_CALL(*source2, olinkObjectName()).RETURN(source2Id);

    ApiGear::ObjectLink::RemoteRegistry registry;
    registry.addSource(source1);

    SECTION("Add object and a node for it") {

        auto node1 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto nodeId1 = node1->getNodeId();

        std::vector<std::string> empty = { };
        REQUIRE(registry.getObjectIds(nodeId1) == empty);

        registry.addNodeForSource(nodeId1, source1Id);
        std::vector<std::string> expectedId = { source1Id };
        REQUIRE(registry.getObjectIds(nodeId1) == expectedId);
        REQUIRE(registry.getSource(source1Id).lock() == source1);
        REQUIRE(registry.getNodes(source1Id)[0].lock() == node1);
    }

    SECTION("One node can be set for many sources")
    {
        auto node1 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto node2 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto nodeId1 = node1->getNodeId();
        registry.addSource(source2);

        REQUIRE(registry.getObjectIds(nodeId1).size() == 0);
        registry.addNodeForSource(nodeId1, source1Id);
        registry.addNodeForSource(nodeId1, source2Id);
        std::string notExisitnigSinkId = "notExisitnigSinkId";
        registry.addNodeForSource(nodeId1, notExisitnigSinkId);
        std::vector<std::string> expectedIds = { source1Id, source2Id }; 
        REQUIRE_THAT(registry.getObjectIds(nodeId1), Catch::Matchers::UnorderedEquals(expectedIds));

        // For all ids for which source was added we're getting same node.
        // For one for which there is no source, node wasn't added.
        REQUIRE(registry.getNodes(source1Id)[0].lock() == node1);
        REQUIRE(registry.getNodes(source2Id)[0].lock() == node1);
        REQUIRE(registry.getNodes(notExisitnigSinkId).size() == 0);
        // For all ids we're getting added objects
        REQUIRE(registry.getSource(source1Id).lock() == source1);
        REQUIRE(registry.getSource(source2Id).lock() == source2);
        REQUIRE(registry.getSource(notExisitnigSinkId).expired() == true);
    }

    SECTION("Many nodes can be added for node. Node can be added only once. Node can be removed.")
    {
        auto node1 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto node2 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto nodeId1 = node1->getNodeId();
        auto nodeId2 = node2->getNodeId();

        registry.addNodeForSource(nodeId1, source1Id);
        auto nodes = registry.getNodes(source1Id);
        REQUIRE(nodes.size() == 1);
        REQUIRE(nodes[0].lock() == node1);

        registry.addNodeForSource(nodeId2, source1Id);
        nodes = registry.getNodes(source1Id);
        REQUIRE(nodes.size() == 2);
        REQUIRE(nodes[0].lock() == node1);
        REQUIRE(nodes[1].lock() == node2);

        registry.addNodeForSource(nodeId2, source1Id);
        nodes = registry.getNodes(source1Id);
        nodes = registry.getNodes(source1Id);
        REQUIRE(nodes.size() == 2);
        REQUIRE(nodes[0].lock() == node1);
        REQUIRE(nodes[1].lock() == node2);

        registry.removeNodeFromSource(nodeId1, source1Id);
        nodes = registry.getNodes(source1Id);
        REQUIRE(nodes.size() == 1);
        REQUIRE(nodes[0].lock() == node2);

    }

    SECTION("Object can be registered for same id only after removing older object (with its node).")
    {
        auto differentSourceForId1 = std::make_shared< SourceObjectMock>();
        ALLOW_CALL(*differentSourceForId1, olinkObjectName()).RETURN(source1Id);

        auto node1 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto node2 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto nodeId1 = node1->getNodeId();

        registry.addNodeForSource(nodeId1, source1Id);

        REQUIRE(registry.getSource(source1Id).lock() == source1);
        REQUIRE(registry.getNodes(source1Id)[0].lock() == node1);

        registry.addSource(differentSourceForId1);
        // Still source1Id registered with a node1 for id
        REQUIRE(registry.getNodes(source1Id)[0].lock() == node1);
        REQUIRE(registry.getSource(source1Id).lock() == source1);

        registry.removeSource(source1Id);
        REQUIRE(registry.getNodes(source1Id).size() == 0);
        REQUIRE(registry.getSource(source1Id).expired() == true);

        registry.addSource(differentSourceForId1);

        // Now differentSourceForId1 is set for id, but no node is assigned.
        REQUIRE(registry.getNodes(source1Id).size() == 0);
        REQUIRE(registry.getSource(source1Id).lock() == differentSourceForId1);
    }

    SECTION("Add node first, then the source")
    {
        auto node1 = ApiGear::ObjectLink::RemoteNode::createRemoteNode(registry);
        auto nodeId1 = node1->getNodeId();
        registry.addNodeForSource(nodeId1, source2Id);
        registry.addSource(source2);

        REQUIRE(registry.getSource(source2Id).lock() == source2);
    }


    SECTION("Removing not existing object doesn't crash")
    {
        registry.removeSource("some otherId");
    }
}