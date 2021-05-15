#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/messages.h"
#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/stdoutlogger.h"
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
    Service service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    Client client("client1");
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

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // register sink object
        client.registry().addObjectSink("demo.Calc", &sink);
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
    Service service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    Client client("client1");
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
    Service service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    Client client("client1");
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
    Service service(&sourceRegistry);
    service.onLog(log.logFunc());
    CalcSource source;

    // setup client
    Client client("client1");
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
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        REQUIRE( sink.events.size() == 0);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
}
