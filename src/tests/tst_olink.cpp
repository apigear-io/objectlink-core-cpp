#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/consolelogger.h"
#include "olink/sinklink.h"
#include "olink/sourcelink.h"
#include "olink/sinknode.h"
#include "olink/sourcenode.h"


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
    SourceNode sourceNode("host1");
    SourceLink sourceLink(sourceNode.nodeName());
    sourceLink.onLog(log.logFunc());
    CalcSource source;

    // setup client
    SinkNode sinkNode("client1");
    SinkLink sinkLink(sinkNode.nodeName());
    sinkLink.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&sourceLink](std::string msg) {
        sourceLink.handleMessage(msg);
    };
    sinkLink.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&sinkLink](std::string msg) {
        sinkLink.handleMessage(msg);
    };
    sourceLink.onWrite(serviceWriteFunc);


    // register source object
    sourceNode.addObjectSource("demo.Calc", &source);
    // register sink object
    sinkLink.sinkNode()->addObjectSink("demo.Calc", &sink);

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // link sink with source
        sinkLink.link("demo.Calc");
        // initalized sink with total=1
        REQUIRE( sink.isReady() == true );
        REQUIRE( sink.total() == 1);
    }
}

TEST_CASE("setProperty")
{
    ConsoleLogger log;
    // setup service
    SourceNode sourceNode("host1");
    SourceLink sourceLink(sourceNode.nodeName());
    sourceLink.onLog(log.logFunc());
    CalcSource source;

    // setup client
    SinkNode sinkNode("client1");
    SinkLink sinkLink(sinkNode.nodeName());
    sinkLink.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc sinkWriteFunc = [&sourceLink](std::string msg) {
        sourceLink.handleMessage(msg);
    };
    sinkLink.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&sinkLink](std::string msg) {
        sinkLink.handleMessage(msg);
    };
    sourceLink.onWrite(sourceWriteFunc);

    // register source object
    sourceNode.addObjectSource("demo.Calc", &source);
    sinkLink.sinkNode()->addObjectSink("demo.Calc", &sink);
    sinkLink.link("demo.Calc");

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
    SourceNode sourceNode("host1");
    SourceLink sourceLink(sourceNode.nodeName());
    sourceLink.onLog(log.logFunc());
    CalcSource source;

    // setup client
    SinkNode sinkNode("client1");
    SinkLink sinkLink(sinkNode.nodeName());
    sinkLink.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc sinkWriteFunc = [&sourceLink](std::string msg) {
        sourceLink.handleMessage(msg);
    };
    sinkLink.onWrite(sinkWriteFunc);

    WriteMessageFunc sourceWriteFunc = [&sinkLink](std::string msg) {
        sinkLink.handleMessage(msg);
    };
    sourceLink.onWrite(sourceWriteFunc);

    // register source object
    sourceNode.addObjectSource("demo.Calc", &source);
    sinkLink.sinkNode()->addObjectSink("demo.Calc", &sink);
    sinkLink.link("demo.Calc");
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
    SourceNode sourceNode("host1");
    SourceLink sourceLink("host1");
    sourceLink.onLog(log.logFunc());
    CalcSource source;

    // setup client
    SinkNode sinkNode("client1");
    SinkLink sinkLink("client1");
    sinkLink.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&sourceLink](std::string msg) {
        sourceLink.handleMessage(msg);
    };
    sinkLink.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&sinkLink](std::string msg) {
        sinkLink.handleMessage(msg);
    };
    sourceLink.onWrite(serviceWriteFunc);


    // register source object
    sourceNode.addObjectSource("demo.Calc", &source);
    sinkNode.addObjectSink("demo.Calc", &sink);
    sinkLink.link("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
}
