#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "olink/messages.h"
#include "olink/protocol.h"
#include "olink/types.h"
#include "olink/client.h"
#include "olink/service.h"
#include "olink/stdoutlogger.h"


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

class IObjectNotifier {
public:
    virtual ~IObjectNotifier() {}
    virtual void notifySignal(std::string name, json args) = 0;
    virtual void notifyPropertyChange(std::string name, json value) = 0;
};

class CalcClientObject: public IClientObjectListener {
public:
    CalcClientObject(ObjectLinkClient *m_client)
        : m_client(m_client)
        , m_total(0)
        , m_isReady(false)
    {
    }
    virtual ~CalcClientObject() override {}
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
    ObjectLinkClient *m_client;
    int m_total;
    bool m_isReady;

};

class CalcServiceObject: public IClientObjectListener {
public:
    CalcServiceObject(IObjectNotifier *notifier)
        : m_notifier(notifier)
        , m_total(0)
    {
    }
    virtual ~CalcServiceObject() override {}

    int add(int value) {
        m_total += value;
        m_notifier->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/add", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total >= 10) {
            m_notifier->notifySignal("demo.Calc/hitUpper", { 10 });
            m_events.push_back({ "demo.Calc/hitUpper", 10 });
        }
        return m_total;
    }

    int sub(int value) {
        m_total -= value;
        m_notifier->notifyPropertyChange("demo.Calc/total", m_total);
        m_events.push_back({ "demo.Calc/sub", value });
        m_events.push_back({ "demo.Calc/total", m_total });
        if(m_total <= 0) {
            m_notifier->notifySignal("demo.Calc/hitLower", { 0 });
            m_events.push_back({ "demo.Calc/hitLower", 0 });
        }
        return m_total;
    }
private:
    int m_total;
    IObjectNotifier* m_notifier;
    std::vector<json> m_events;

};


TEST_CASE("client")
{
    StdoutLogger log;
    LoopbackWriter writer;
    ObjectLinkClient client(&writer, &log, MessageFormat::JSON);
    CalcClientObject obj(&client);

    SECTION("link ->, <- init") {
        client.addObject("demo.Calc", &obj);
        REQUIRE( obj.isReady() == true );
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
