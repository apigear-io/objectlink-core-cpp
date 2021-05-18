#pragma once

#include "olink/core/listeners.h"
#include "olink/core/protocol.h"
#include "olink/sinktypes.h"
#include "olink/sourcetypes.h"

namespace ApiGear { namespace ObjectLink {

class ObjectLink: public DefaultProtocolListener, public IMessageHandler {
public:
    ObjectLink(std::string name);
    void onWrite(WriteMessageFunc func);
    void onLog(WriteLogFunc func);
    std::string nodeName() const;
    virtual void emitWrite(json j);
    void emitLog(LogLevel level, std::string msg);
public: // IMessageHandler
    void handleMessage(std::string data) override;
private:
    WriteMessageFunc m_writeFunc;
    WriteLogFunc m_logFunc;
    MessageConverter m_converter;
    Protocol m_protocol;
    std::string m_nodeName;
};

} } // ApiGear::ObjectLink
