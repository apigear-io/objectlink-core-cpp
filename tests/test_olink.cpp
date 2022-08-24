#include <catch2/catch.hpp>

#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/consolelogger.h"
#include "olink/clientnode.h"
#include "olink/remotenode.h"


#include "nlohmann/json.hpp"

#include <list>
#include <string>
#include <memory>

#include "sinkobject.hpp"
#include "sourceobject.hpp"

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;


TEST_CASE("link")
{
    RemoteRegistry registry;
    registry.onLog(ConsoleLogger::logFunc());
    ClientRegistry clientRegistry;
    clientRegistry.onLog(ConsoleLogger::logFunc());
    // setup service
    RemoteNode remote(registry);

    CalcSource source(registry);

    // setup client
    ClientNode client(clientRegistry);
    client.onLog(ConsoleLogger::logFunc());
    CalcSink sink(clientRegistry);

    WriteMessageFunc clientWriteFunc = [&remote](std::string msg) {
        remote.handleMessage(msg);
    };
    client.onWrite(clientWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    remote.onWrite(sourceWriteFunc);


    // register source object
    remote.addObjectSource(&source);
    // register sink object
    clientRegistry.addObject(sink);

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // link sink with source
        client.linkRemote("demo.Calc");
        // initalized sink with total=1
        REQUIRE( sink.isReady() == true );
        REQUIRE( sink.total() == 1);
    }
//    remote.removeObjectSource(&source);
//    client.removeObject(olinkObjectName());
}

TEST_CASE("setProperty")
{
    ConsoleLogger log;
    // setup service
    RemoteRegistry registry;
    RemoteNode remote(registry);
    remote.onLog(log.logFunc());
    CalcSource source(registry);

    // setup client
//    ObjectSinkRegistry clientRegistry("client1");
    ClientRegistry clientRegistry;
    ClientNode client(clientRegistry);
    client.onLog(log.logFunc());
    CalcSink sink(clientRegistry);

    WriteMessageFunc sinkWriteFunc = [&remote](std::string msg) {
        remote.handleMessage(msg);
    };
    client.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    remote.onWrite(sourceWriteFunc);

    // register source object
    remote.addObjectSource(&source);
    clientRegistry.addObject(sink);
    client.linkRemote("demo.Calc");

    REQUIRE( sink.isReady() == true );
    SECTION("set property") {
        REQUIRE( sink.total() == 1);
        sink.setTotal(2);
        REQUIRE( sink.total() == 2);
        sink.setTotal(3);
        REQUIRE( sink.total() == 3);
    }
//    remote.removeObjectSource(&source);
//    client.removeObject(olinkObjectName());
}

TEST_CASE("signal")
{
    ConsoleLogger log;
    // setup service
    RemoteRegistry registry;
    RemoteNode remote(registry);
    remote.onLog(log.logFunc());
    CalcSource source(registry);

    // setup client
//    ObjectSinkRegistry clientRegistry("client1");
    ClientRegistry clientRegistry;
    ClientNode client(clientRegistry);
    client.onLog(log.logFunc());
    CalcSink sink(clientRegistry);

    WriteMessageFunc sinkWriteFunc = [&remote](std::string msg) {
        remote.handleMessage(msg);
    };
    client.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    remote.onWrite(sourceWriteFunc);

    // register source object
    remote.addObjectSource(&source);
    clientRegistry.addObject(sink);
    client.linkRemote("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("signal") {
        REQUIRE( sink.events.size() == 0);
        source.notifyShutdown(10);
        REQUIRE( sink.events.size() == 1);
    }
//    remote.removeObjectSource(&source);
//    client.removeObject(olinkObjectName());
}


TEST_CASE("invoke")
{
    ConsoleLogger log;
    // setup service
//    ObjectSourceRegistry sourceRegistry("host1");
    RemoteRegistry registry;
    RemoteNode remote(registry);
    remote.onLog(log.logFunc());
    CalcSource source(registry);

    // setup client
//    ObjectSinkRegistry sinkRegistry("client1");
    ClientRegistry clientRegistry;
    ClientNode client(clientRegistry);
    client.onLog(log.logFunc());
    CalcSink sink(clientRegistry);

    WriteMessageFunc clientWriteFunc = [&remote](std::string msg) {
        remote.handleMessage(msg);
    };
    client.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    remote.onWrite(serviceWriteFunc);


    // register source object
    remote.addObjectSource(&source);
    clientRegistry.addObject(sink);
    client.linkRemote("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
//    remote.removeObjectSource(&source);
//    client.removeObject(olinkObjectName());
}
