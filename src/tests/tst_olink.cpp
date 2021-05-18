#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/consolelogger.h"
#include "olink/sink/sinknode.h"
#include "olink/sink/sinkregistry.h"
#include "olink/source/sourcenode.h"
#include "olink/source/sourceregistry.h"


#include "nlohmann/json.hpp"

#include <list>
#include <string>

#include "sinkobject.hpp"
#include "sourceobject.hpp"

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;


TEST_CASE("link")
{
    ConsoleLogger log;
    // setup service
    ObjectSourceRegistry hostRegistry("host1");
    ObjectSourceNode sourceNode(hostRegistry.name());
    sourceNode.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ObjectSinkRegistry sinkRegistry("client1");
    ObjectSinkNode sinkNode(sinkRegistry.name());
    sinkNode.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc sinkWriteFunc = [&sourceNode](std::string msg) {
        sourceNode.handleMessage(msg);
    };
    sinkNode.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&sinkNode](std::string msg) {
        sinkNode.handleMessage(msg);
    };
    sourceNode.onWrite(sourceWriteFunc);


    // register source object
    hostRegistry.addSource("demo.Calc", &source);
    // register sink object
    sinkNode.registry()->addObjectSink("demo.Calc", &sink);

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // link sink with source
        sinkNode.link("demo.Calc");
        // initalized sink with total=1
        REQUIRE( sink.isReady() == true );
        REQUIRE( sink.total() == 1);
    }
}

TEST_CASE("setProperty")
{
    ConsoleLogger log;
    // setup service
    ObjectSourceRegistry hostRegistry("host1");
    ObjectSourceNode sourceNode(hostRegistry.name());
    sourceNode.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ObjectSinkRegistry clientRegistry("client1");
    ObjectSinkNode sinkNode(clientRegistry.name());
    sinkNode.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc sinkWriteFunc = [&sourceNode](std::string msg) {
        sourceNode.handleMessage(msg);
    };
    sinkNode.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&sinkNode](std::string msg) {
        sinkNode.handleMessage(msg);
    };
    sourceNode.onWrite(sourceWriteFunc);

    // register source object
    hostRegistry.addSource("demo.Calc", &source);
    sinkNode.registry()->addObjectSink("demo.Calc", &sink);
    sinkNode.link("demo.Calc");

    REQUIRE( sink.isReady() == true );
    SECTION("set property") {
        REQUIRE( sink.total() == 1);
        sink.setTotal(2);
        REQUIRE( sink.total() == 2);
        sink.setTotal(3);
        REQUIRE( sink.total() == 3);
    }
}

TEST_CASE("signal")
{
    ConsoleLogger log;
    // setup service
    ObjectSourceRegistry hostRegistry("host1");
    ObjectSourceNode sourceNode(hostRegistry.name());
    sourceNode.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ObjectSinkRegistry clientRegistry("client1");
    ObjectSinkNode sinkNode(clientRegistry.name());
    sinkNode.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc sinkWriteFunc = [&sourceNode](std::string msg) {
        sourceNode.handleMessage(msg);
    };
    sinkNode.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&sinkNode](std::string msg) {
        sinkNode.handleMessage(msg);
    };
    sourceNode.onWrite(sourceWriteFunc);

    // register source object
    hostRegistry.addSource("demo.Calc", &source);
    clientRegistry.addObjectSink("demo.Calc", &sink);
    sinkNode.link("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("signal") {
        REQUIRE( sink.events.size() == 0);
        source.notifyShutdown(10);
        REQUIRE( sink.events.size() == 1);
    }
}


TEST_CASE("invoke")
{
    ConsoleLogger log;
    // setup service
    ObjectSourceRegistry sourceRegistry("host1");
    ObjectSourceNode sourceNode("host1");
    sourceNode.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ObjectSinkRegistry sinkRegistry("client1");
    ObjectSinkNode sinkNode("client1");
    sinkNode.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&sourceNode](std::string msg) {
        sourceNode.handleMessage(msg);
    };
    sinkNode.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&sinkNode](std::string msg) {
        sinkNode.handleMessage(msg);
    };
    sourceNode.onWrite(serviceWriteFunc);


    // register source object
    sourceRegistry.addSource("demo.Calc", &source);
    sinkRegistry.addObjectSink("demo.Calc", &sink);
    sinkNode.link("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
}
