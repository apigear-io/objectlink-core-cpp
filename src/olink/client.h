#pragma once
#include "olink/sinktypes.h"
#include "olink/sinkregistry.h"
#include "olink/core/listeners.h"
#include "olink/core/messages.h"

namespace ApiGear { namespace ObjectLink {

class Client : public IMessagesListener, public IMessageHandler, public IClient
{
public:
    Client(std::string name);
    void onWrite(WriteMessageFunc func);
    void onLog(LogWriterFunc func);
    std::string name() const;
    SinkRegistry& registry();
protected:
    void emitWrite(json j);
    void emitLog(LogLevel level, std::string msg);
    int nextId();
public: // IMessageHandler
    void handleMessage(std::string data) override;
public: // IObjectLinkClient
    void invoke(std::string name, json args, InvokeReplyFunc func) override;
    void setProperty(std::string name, json value) override;
protected: // IMessageListener
    void handleLink(std::string name) override;
    void handleUnlink(std::string name) override;
    void handleInit(std::string name, json props) override;
    void handleSetProperty(std::string name, json value) override;
    void handlePropertyChange(std::string name, json value) override;
    void handleInvoke(int requestId, std::string name, json args) override;
    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handleError(int msgType, int requestId, std::string error) override;
private:
    WriteMessageFunc m_writeFunc;
    LogWriterFunc m_logFunc;
    MessageConverter m_converter;
    Messages m_messages;
    int m_nextId;
    std::map<int,InvokeReplyFunc> m_invokesPending;
    std::string m_name;
    SinkRegistry m_registry;
};

} } // ApiGear::ObjectLink
