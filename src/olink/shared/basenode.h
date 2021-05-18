#pragma once

#include "olink/core/listeners.h"
#include "olink/core/protocol.h"

namespace ApiGear { namespace ObjectLink {

class BaseNode: public DefaultObjectLinkProtocolListener, public IMessageHandler {
public:
    BaseNode(std::string name);
    void onWrite(WriteMessageFunc func);
    void onLog(WriteLogFunc func);
    std::string name() const;
    virtual void emitWrite(json j);
    void emitLog(LogLevel level, std::string msg);
public: // IMessageHandler
    void handleMessage(std::string data) override;
private:
    WriteMessageFunc m_writeFunc;
    WriteLogFunc m_logFunc;
    MessageConverter m_converter;
    ObjectLinkProtocol m_protocol;
    std::string m_name;
};

} } // ApiGear::ObjectLink
