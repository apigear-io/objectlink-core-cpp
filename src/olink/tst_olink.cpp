#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/core/messages.h"
#include "olink/core/protocol.h"
#include "olink/core/types.h"
#include "olink/core/stdoutlogger.h"
#include "olink/session.h"


#include "nlohmann/json.hpp"

#include <iostream>
#include <list>
#include <string>

using json = nlohmann::json;
using namespace std;
using namespace ApiGear::ObjectLink;

class LoopbackWriter: public IMessageWriter {
public:
    LoopbackWriter(): m_handler(nullptr) {}
    void writeMessage(std::string message) override {
        assert(m_handler);
        cout << "handle message {}" << message;
        m_handler->handleMessage(message);
    }
    void setHandler(IMessageHandler *handler) { m_handler = handler; }
private:
    IMessageHandler *m_handler;
};


class CalcSink: public IObjectLinkSink {
public:
    CalcSink(IObjectLinkClient *m_client)
        : m_client(m_client)
        , m_total(0)
        , m_isReady(false)
    {
    }
    virtual ~CalcSink() override {}
    int add(int a) {
        m_client->invoke("demo.Calc/add", { a });
        return -1;
    }
    int sub(int a) {
        m_client->invoke("demo.Calc/sub", { a });
        return -1;
    }
    bool isReady() const {
        return m_isReady;
    }
    // IClientObjectHandler interface
public:
    void onSignal(string name, json args) override {
        cout << __func__ << name << args.dump();

    }
    void onPropertyChanged(string name, json value) override {
        cout << __func__ << name << value.dump();
        if(name == "total") {
            int total = value.get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }

    }
    void onInit(string name, json props) override {
        cout << __func__ << name << props.dump();
        m_isReady = true;
        if(props.contains("total")) {
            int total = props["total"].get<int>();
            if(m_total != total) {
                m_total = total;
            }
        }
    }
private:
    IObjectLinkClient *m_client;
    int m_total;
    bool m_isReady;

};

class CalcSource: public IObjectLinkSource {
public:
    CalcSource()
        : m_service(nullptr)
        , m_total(0)
    {
    }
    virtual ~CalcSource() override {}

    int add(int value) {
        if(!m_service) {
            return 0;
        }
        m_total += value;
        m_service->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/add", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total >= 10) {
            m_service->notifySignal("demo.Calc/hitUpper", { 10 });
            m_events.push_back({ "demo.Calc/hitUpper", 10 });
        }
        return m_total;
    }

    int sub(int value) {
        if(!m_service) {
            return 0;
        }
        m_total -= value;
        m_service->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/sub", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total <= 0) {
            m_service->notifySignal("demo.Calc/hitLower", { 0 });
            m_events.push_back({ "demo.Calc/hitLower", 0 });
        }
        return m_total;
    }
    // IServiceObjectListener interface
public:
    string getObjectName() override {
        return "demo.Calc";
    }
    json invoke(string name, json args) override {
        std::cout << __func__ << name << args.dump();
        return "hello";
    }
    void setProperty(string name, json value) override {
        std::cout << __func__ << name << value.dump();
    }
    void linked(string name, IObjectLinkService *service) override {
        std::cout << __func__ << name;
        m_service = service;
    }
    void unlinked(string name) override
    {
        std::cout << __func__ << name;
        m_service = nullptr;
    }
    json collectProperties() override
    {
        return {{ "total", m_total }};
    }
private:
    IObjectLinkService* m_service;
    int m_total;
    std::vector<json> m_events;
};


TEST_CASE("client")
{
    StdoutLogger log;
    LoopbackWriter writer;
    ObjectLinkSession client(&writer, MessageFormat::JSON, &log);
    CalcSink co(&client);

    ObjectLinkSession service(&writer, MessageFormat::JSON, &log);
    CalcSource so;
    service.addObjectSource("demo.Calc", &so);


    SECTION("link ->, <- init") {
        client.addObjectSink("demo.Calc", &co);
        REQUIRE( co.isReady() == true );
    }
//    obj.add(4);
//    obj.sub(5);

}

//TEST_CASE("rpc")
//{
//    StdoutLogger log;
//    LoopbackWriter routerWriter;
//    Router r(&routerWriter, &log);

//    LoopbackWriter sessionWriter;
//    MockSessionListener listener;
//    Session s(&listener, &sessionWriter, &log, MessageFormat::JSON);

//    routerWriter.setHandler(&s);
//    sessionWriter.setHandler(&r);
//    s.init("realm1");
//    REQUIRE( listener.hasJoined == true );

//    SECTION("call ->, <- response") {
//        bool called = false;
//        ResponseFunc func = [&](ResponseArg arg) mutable {
//            cout << "response callback";
//            called = true;
//        };
//        s.doCall("increment", {}, {{}}, func);
//        REQUIRE( called == true );
//    }
//}

//TEST_CASE("pub sub")
//{
//    StdoutLogger log;

//    LoopbackWriter routerWriter;
//    Router r(&routerWriter, &log);

//    LoopbackWriter sessionWriter;
//    MockSessionListener listener;
//    Session s(&listener, &sessionWriter, &log, MessageFormat::JSON);

//    routerWriter.setHandler(&s);
//    sessionWriter.setHandler(&r);
//    s.init("realm1");
//    REQUIRE( listener.hasJoined == true );
//    listener.events.clear();
//    r.hasSubscribed = false;
//    bool eventCalled = false;
//    EventFunc func = [&](EventArg arg) { eventCalled = true; };
//    SECTION("subscribe ->, publish ->") {
//        REQUIRE( listener.events.size() == 0);
//        s.doSubscribe("weather", func);
//        REQUIRE( listener.events.size() == 0);
//        s.doPublish("weather", {}, {{}});
//        REQUIRE( listener.events.size() == 1);
//        REQUIRE( eventCalled == true );
//    }
//    SECTION("publish ->, no subscribe") {
//        REQUIRE( listener.events.size() == 0);
//        s.doPublish("weather", {}, {{}});
//        REQUIRE( listener.events.size() == 0);
//    }

//}
