#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/consolelogger.h"
#include "olink/client.h"
#include "olink/service.h"
#include "olink/sourceregistry.h"
#include "olink/sinkregistry.h"


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
    SourceRegistry sourceRegistry("host1");
    ServiceIO service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ClientIO client("client1");
    client.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&service](std::string msg) {
        service.handleMessage(msg);
    };
    client.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    service.onWrite(serviceWriteFunc);


    // register source object
    sourceRegistry.addObjectSource("demo.Calc", &source);
    // register sink object
    client.registry().addObjectSink("demo.Calc", &sink);

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // link sink with source
        client.link("demo.Calc");
        // initalized sink with total=1
        REQUIRE( sink.isReady() == true );
        REQUIRE( sink.total() == 1);
    }
}

TEST_CASE("setProperty")
{
    ConsoleLogger log;
    // setup service
    SourceRegistry sourceRegistry("host1");
    ServiceIO service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ClientIO client("client1");
    client.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&service](std::string msg) {
        service.handleMessage(msg);
    };
    client.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    service.onWrite(serviceWriteFunc);

    // register source object
    sourceRegistry.addObjectSource("demo.Calc", &source);
    client.registry().addObjectSink("demo.Calc", &sink);
    client.link("demo.Calc");

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
    SourceRegistry sourceRegistry("host1");
    ServiceIO service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ClientIO client("client1");
    client.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&service](std::string msg) {
        service.handleMessage(msg);
    };
    client.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&client](std::string msg) {
        client.handleMessage(msg);
    };
    service.onWrite(serviceWriteFunc);

    // register source object
    sourceRegistry.addObjectSource("demo.Calc", &source);
    client.registry().addObjectSink("demo.Calc", &sink);
    client.link("demo.Calc");
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
    SourceRegistry sourceRegistry("host1");
    ServiceIO serviceIO(&sourceRegistry);
    serviceIO.onLog(log.logFunc());
    CalcSource source;

    // setup client
    ClientIO clientIO("client1");
    clientIO.onLog(log.logFunc());
    CalcSink sink;

    WriteMessageFunc clientWriteFunc = [&serviceIO](std::string msg) {
        serviceIO.handleMessage(msg);
    };
    clientIO.onWrite(clientWriteFunc);

    WriteMessageFunc serviceWriteFunc = [&clientIO](std::string msg) {
        clientIO.handleMessage(msg);
    };
    serviceIO.onWrite(serviceWriteFunc);

    // register source object
    sourceRegistry.addObjectSource("demo.Calc", &source);
    clientIO.registry().addObjectSink("demo.Calc", &sink);
    clientIO.link("demo.Calc");
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        REQUIRE( sink.events.size() == 0);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
}
