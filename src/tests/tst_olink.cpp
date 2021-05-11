#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/messages.h"
#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/stdoutlogger.h"
#include "olink/session.h"


#include "nlohmann/json.hpp"

#include <list>
#include <string>
#include "spdlog/spdlog.h"

#include "sinkobject.hpp"
#include "sourceobject.hpp"

using json = nlohmann::json;
using namespace ApiGear::ObjectLink;

TEST_CASE("link")
{
    spdlog::set_level(spdlog::level::trace);
    StdoutLogger log;
    // setup service
    LoopbackWriter serviceWriter;
    ObjectLinkSession service(&serviceWriter, MessageFormat::JSON, &log);
    CalcSource source;

    // setup client
    LoopbackWriter clientWriter;
    ObjectLinkSession client(&clientWriter, MessageFormat::JSON, &log);
    CalcSink sink(&client);


    clientWriter.setHandler(&service);
    serviceWriter.setHandler(&client);

    // register source object
    service.addObjectSource("demo.Calc", &source);

    SECTION("link ->, <- init") {
        // not initalized sink, with total=0
        REQUIRE( sink.isReady() == false );
        REQUIRE( sink.total() == 0);
        // register sink object
        client.addObjectSink("demo.Calc", &sink);
        // initalized sink with total=1
        REQUIRE( sink.isReady() == true );
        REQUIRE( sink.total() == 1);
    }
}

TEST_CASE("setProperty")
{
    spdlog::set_level(spdlog::level::trace);
    StdoutLogger log;
    // setup service
    LoopbackWriter serviceWriter;
    ObjectLinkSession service(&serviceWriter, MessageFormat::JSON, &log);
    CalcSource source;

    // setup client
    LoopbackWriter clientWriter;
    ObjectLinkSession client(&clientWriter, MessageFormat::JSON, &log);
    CalcSink sink(&client);


    clientWriter.setHandler(&service);
    serviceWriter.setHandler(&client);

    // register source object
    service.addObjectSource("demo.Calc", &source);
    client.addObjectSink("demo.Calc", &sink);

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
    spdlog::set_level(spdlog::level::trace);
    StdoutLogger log;
    // setup service
    LoopbackWriter serviceWriter;
    ObjectLinkSession service(&serviceWriter, MessageFormat::JSON, &log);
    CalcSource source;

    // setup client
    LoopbackWriter clientWriter;
    ObjectLinkSession client(&clientWriter, MessageFormat::JSON, &log);
    CalcSink sink(&client);


    clientWriter.setHandler(&service);
    serviceWriter.setHandler(&client);

    // register source object
    service.addObjectSource("demo.Calc", &source);
    client.addObjectSink("demo.Calc", &sink);

    REQUIRE( sink.isReady() == true );

    SECTION("signal") {
        REQUIRE( sink.events.size() == 0);
        source.notifyShutdown(10);
        REQUIRE( sink.events.size() == 1);
    }
}


TEST_CASE("invoke")
{
    spdlog::set_level(spdlog::level::trace);
    StdoutLogger log;
    // setup client
    LoopbackWriter clientWriter;
    ObjectLinkSession client(&clientWriter, MessageFormat::JSON, &log);
    CalcSink sink(&client);

    // setup service
    LoopbackWriter serviceWriter;
    ObjectLinkSession service(&serviceWriter, MessageFormat::JSON, &log);
    CalcSource source;

    clientWriter.setHandler(&service);
    serviceWriter.setHandler(&client);

    // register source object
    service.addObjectSource("demo.Calc", &source);
    client.addObjectSink("demo.Calc", &sink);
    REQUIRE( sink.isReady() == true );

    SECTION("invoke") {
        REQUIRE( sink.total() == 1);
        REQUIRE( sink.events.size() == 0);
        sink.add(5);
        REQUIRE( sink.total() == 6);
    }
}
