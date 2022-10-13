#include <catch2/catch.hpp>

#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/consolelogger.h"
#include "olink/clientnode.h"
#include "olink/remotenode.h"
#include "olink/remoteregistry.h"

#include "sinkobject.hpp"
#include "sourceobject.hpp"

#include "nlohmann/json.hpp"
#include <string>
#include <memory>


using json = nlohmann::json;
using namespace ApiGear::ObjectLink;


TEST_CASE("link")
{
    RemoteRegistry registry;
    registry.onLog(ConsoleLogger::logFunc());
    ClientRegistry clientRegistry;
    clientRegistry.onLog(ConsoleLogger::logFunc());

    // setup service
    auto remote = RemoteNode::createRemoteNode(registry);
    auto source = std::make_shared<CalcSource>(registry);
    registry.addSource(source);

    // setup client
    auto client = ClientNode::create(clientRegistry);
    client->onLog(ConsoleLogger::logFunc());
    auto sink = std::make_shared<CalcSink>(clientRegistry);
    clientRegistry.addSink(sink);

    WriteMessageFunc clientWriteFunc = [&remote](std::string msg) {
        remote->handleMessage(msg);
    };
    client->onWrite(clientWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client->handleMessage(msg);
    };
    remote->onWrite(sourceWriteFunc);

    SECTION("link ->, <- init") {
        // not initialized sink, with total=0
        REQUIRE( sink->isReady() == false );
        REQUIRE( sink->total() == 0);
        // link sink with source
        client->linkRemote("demo.Calc");
        // initialized sink with total=1
        REQUIRE( sink->isReady() == true );
        REQUIRE( sink->total() == 1);
    }

    registry.removeSource(source->olinkObjectName());
    clientRegistry.removeSink(sink->olinkObjectName());
}

TEST_CASE("setProperty")
{
    ConsoleLogger log;
    // setup service
    RemoteRegistry registry;
    auto remote = RemoteNode::createRemoteNode(registry);
    remote->onLog(log.logFunc());
    auto source = std::make_shared<CalcSource>(registry);
    registry.addSource(source);

    // setup client
    ClientRegistry clientRegistry;
    auto client = ClientNode::create(clientRegistry);
    client->onLog(log.logFunc());
    auto sink = std::make_shared<CalcSink>(clientRegistry);
    clientRegistry.addSink(sink);

    WriteMessageFunc sinkWriteFunc = [&remote](std::string msg) {
        remote->handleMessage(msg);
    };
    client->onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client->handleMessage(msg);
    };
    remote->onWrite(sourceWriteFunc);

    // register source object
    registry.addSource(source);
    clientRegistry.addSink(sink);
    client->linkRemote("demo.Calc");

    REQUIRE( sink->isReady() == true );
    SECTION("set property") {
        REQUIRE( sink->total() == 1);
        sink->setTotal(2);
        REQUIRE( sink->total() == 2);
        sink->setTotal(3);
        REQUIRE( sink->total() == 3);
    }

    registry.removeSource(source->olinkObjectName());
    clientRegistry.removeSink(sink->olinkObjectName());
}

TEST_CASE("signal")
{
    ConsoleLogger log;
    // setup service
    RemoteRegistry registry;
    auto remote = RemoteNode::createRemoteNode(registry);
    remote->onLog(log.logFunc());
    auto source = std::make_shared<CalcSource>(registry);
    registry.addSource(source);

    // setup client
    ClientRegistry clientRegistry;
    auto client = ClientNode::create(clientRegistry);
    client->onLog(log.logFunc());
    auto sink = std::make_shared<CalcSink>(clientRegistry);
    clientRegistry.addSink(sink);

    WriteMessageFunc sinkWriteFunc = [&remote](std::string msg) {
        remote->handleMessage(msg);
    };
    client->onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client->handleMessage(msg);
    };
    remote->onWrite(sourceWriteFunc);

    // register source object
    registry.addSource(source);
    clientRegistry.addSink(sink);
    client->linkRemote("demo.Calc");
    REQUIRE( sink->isReady() == true );

    SECTION("signal") {
        REQUIRE( sink->events.size() == 0);
        source->notifyShutdown(10);
        REQUIRE( sink->events.size() == 1);
    }
    registry.removeSource(source->olinkObjectName());
    clientRegistry.removeSink(sink->olinkObjectName());
}


TEST_CASE("invoke")
{
    ConsoleLogger log;
    // setup service
    RemoteRegistry registry;
    auto remote = RemoteNode::createRemoteNode(registry);
    remote->onLog(log.logFunc());
    auto source = std::make_shared<CalcSource>(registry);
    registry.addSource(source);

    // setup client
    ClientRegistry clientRegistry;
    auto client = ClientNode::create(clientRegistry);
    client->onLog(log.logFunc());
    auto sink = std::make_shared<CalcSink>(clientRegistry);
    clientRegistry.addSink(sink);

    WriteMessageFunc clientWriteFunc = [&remote](std::string msg) {
        remote->handleMessage(msg);
    };
    client->onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&client](std::string msg) {
        client->handleMessage(msg);
    };
    remote->onWrite(serviceWriteFunc);


    // register source object
    registry.addSource(source);
    clientRegistry.addSink(sink);
    client->linkRemote("demo.Calc");
    REQUIRE( sink->isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink->total() == 1);
        sink->add(5);
        REQUIRE( sink->total() == 6);
    }
    registry.removeSource(source->olinkObjectName());
    clientRegistry.removeSink(sink->olinkObjectName());
}
