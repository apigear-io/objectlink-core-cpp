#pragma once
#include "olink/sinktypes.h"
#include "olink/sinkregistry.h"
#include "olink/core/listeners.h"
#include "olink/core/protocol.h"

namespace ApiGear { namespace ObjectLink {

class ClientIO : public EmptyMessagesListener, public IMessageHandler, public IClient
{
public:
    ClientIO(std::string name);
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
    void link(std::string name) override;
    void unlink(std::string name) override;
    void invoke(std::string name, json args=json{}, InvokeReplyFunc func=nullptr) override;
    void setProperty(std::string name, json value) override;
protected: // IMessageListener
    void handleInit(std::string name, json props) override;
    void handlePropertyChange(std::string name, json value) override;    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handleError(int msgType, int requestId, std::string error) override;
private:
    WriteMessageFunc m_writeFunc;
    LogWriterFunc m_logFunc;
    MessageConverter m_converter;
    Protocol m_protocol;
    int m_nextId;
    std::map<int,InvokeReplyFunc> m_invokesPending;
    std::string m_name;
    SinkRegistry m_registry;
};

} } // ApiGear::ObjectLink
