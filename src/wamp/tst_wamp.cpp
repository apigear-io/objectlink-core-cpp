#pragma clang diagnostic ignored "-Wweak-vtables"
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "wamp/messages.h"
#include "wamp/protocol.h"
#include "wamp/types.h"
#include "wamp/session.h"
#include "wamp/stdoutlogger.h"


#include "nlohmann/json.hpp"

#include <iostream>
#include <list>
#include <string>

using json = nlohmann::json;
using namespace std;
using namespace ApiGear::WAMP;

class Router : public EmptyProtocolListener, public IMessageHandler {
public:
    Router(IMessageWriter *writer, ILogger* log)
        : m_protocol(writer, log, MessageFormat::JSON, this), hasSubscribed(false) {}

    void handleCall(Id requestId, Options options, string procedure, Arguments arguments, ArgumentsKw argumentsKw) override {
        cout << "router handle call " << procedure;
        // return response
        m_protocol.writeMessage(Message::resultMessage(requestId, options, arguments, argumentsKw));
    }
    void handleMessage(string message) override {
        m_protocol.handleMessage(message);
    }
    void handleHello(string realm, Details details) override {
        m_protocol.writeMessage(Message::welcomeMessage(123, {{}}));
    }
    void handleSubscribe(Id requestId, Options options, string topic) override
    {
        cout << "handle subscribe " << requestId;
        hasSubscribed = true;
        m_protocol.writeMessage(Message::subscribedMessage(requestId, 123));
    }
    void handlePublish(Id requestId, Options options, string topic, Arguments arguments, ArgumentsKw argumentsKw) override {
        cout << "handle publishL "  << requestId << topic;
        if(hasSubscribed) {
            m_protocol.writeMessage(Message::publishedMessage(requestId, 234));
            m_protocol.writeMessage(Message::eventMessage(123, 234, {{}}, {}, {{}}));
        }
    }
public:
    Protocol m_protocol;
    bool hasSubscribed;
    };

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


class MockSessionListener: public ISessionListener {
public:
    MockSessionListener(): hasJoined(false) {}
    void onError(std::string error) override {}
    void onEvent(std::string topic, Arguments args, ArgumentsKw kwargs) override {
        events.push_back(topic);
    }
    void onJoin() override{ hasJoined = true; }
public:
    bool hasJoined;
    std::vector<string> events;
};


TEST_CASE("session")
{
    StdoutLogger log;

    LoopbackWriter routerWriter;
    Router r(&routerWriter, &log);

    LoopbackWriter sessionWriter;
    MockSessionListener listener;

    Session s(&listener, &sessionWriter, &log, MessageFormat::JSON);

    routerWriter.setHandler(&s);
    sessionWriter.setHandler(&r);

    SECTION("hello ->, <- welcome") {
        s.init("realm1");
        REQUIRE( listener.hasJoined == true );

    }
}

TEST_CASE("rpc")
{
    StdoutLogger log;
    LoopbackWriter routerWriter;
    Router r(&routerWriter, &log);

    LoopbackWriter sessionWriter;
    MockSessionListener listener;
    Session s(&listener, &sessionWriter, &log, MessageFormat::JSON);

    routerWriter.setHandler(&s);
    sessionWriter.setHandler(&r);
    s.init("realm1");
    REQUIRE( listener.hasJoined == true );

    SECTION("call ->, <- response") {
        bool called = false;
        ResponseFunc func = [&](ResponseArg arg) mutable {
            cout << "response callback";
            called = true;
        };
        s.doCall("increment", {}, {{}}, func);
        REQUIRE( called == true );
    }
}

TEST_CASE("pub sub")
{
    StdoutLogger log;

    LoopbackWriter routerWriter;
    Router r(&routerWriter, &log);

    LoopbackWriter sessionWriter;
    MockSessionListener listener;
    Session s(&listener, &sessionWriter, &log, MessageFormat::JSON);

    routerWriter.setHandler(&s);
    sessionWriter.setHandler(&r);
    s.init("realm1");
    REQUIRE( listener.hasJoined == true );
    listener.events.clear();
    r.hasSubscribed = false;
    bool eventCalled = false;
    EventFunc func = [&](EventArg arg) { eventCalled = true; };
    SECTION("subscribe ->, publish ->") {
        REQUIRE( listener.events.size() == 0);
        s.doSubscribe("weather", func);
        REQUIRE( listener.events.size() == 0);
        s.doPublish("weather", {}, {{}});
        REQUIRE( listener.events.size() == 1);
        REQUIRE( eventCalled == true );
    }
    SECTION("publish ->, no subscribe") {
        REQUIRE( listener.events.size() == 0);
        s.doPublish("weather", {}, {{}});
        REQUIRE( listener.events.size() == 0);
    }

}
