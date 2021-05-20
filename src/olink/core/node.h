#pragma once

#include "listeners.h"
#include "protocol.h"

namespace ApiGear { namespace ObjectLink {

class BaseNode: public Base, public IProtocolListener, public IMessageHandler {
public:
    BaseNode();
    void onWrite(WriteMessageFunc func);
    void onLog(WriteLogFunc func);
    virtual void emitWrite(json j);
    void emitLog(LogLevel level, std::string msg);
public: // IMessageHandler
    void handleMessage(std::string data) override;
public: // IProtocolListener
    void handleLink(std::string name) override;
    void handleUnlink(std::string name) override;
    void handleInvoke(int requestId, std::string name, json args) override;
    void handleSetProperty(std::string name, json value) override;
    void handleInit(std::string name, json props) override;
    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handlePropertyChange(std::string name, json value) override;
    void handleError(int msgType, int requestId, std::string error) override;
private:
    WriteMessageFunc m_writeFunc;
    WriteLogFunc m_logFunc;
    MessageConverter m_converter;
    Protocol m_protocol;
};

} } // ApiGear::ObjectLink
