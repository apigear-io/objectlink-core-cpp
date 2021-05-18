#include "objectlink.h"

namespace ApiGear { namespace ObjectLink {

ObjectLink::ObjectLink(std::string name)
    : m_writeFunc(nullptr)
    , m_logFunc(nullptr)
    , m_converter(MessageFormat::JSON)
    , m_protocol(this)
    , m_nodeName(name)
{
}

void ObjectLink::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}

void ObjectLink::emitWrite(json msg)
{
    const std::string& data = m_converter.toString(msg);
    emitLog(LogLevel::Debug, "writeMessage " + msg.dump());
    if(m_writeFunc) {
        m_writeFunc(data);
    } else {
        emitLog(LogLevel::Warning, "no writer set, can not write");
    }
}

void ObjectLink::onLog(WriteLogFunc func)
{
    m_logFunc = func;
}

void ObjectLink::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}


void ObjectLink::handleMessage(std::string data)
{
    const json& j = m_converter.fromString(data);
    m_protocol.handleMessage(j);
}

std::string ObjectLink::nodeName() const
{
    return m_nodeName;
}



} } // ApiGear::ObjectLink
