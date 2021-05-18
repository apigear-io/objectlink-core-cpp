#include "basenode.h"

namespace ApiGear { namespace ObjectLink {

BaseNode::BaseNode(std::string name)
    : m_writeFunc(nullptr)
    , m_logFunc(nullptr)
    , m_converter(MessageFormat::JSON)
    , m_protocol(this)
    , m_name(name)
{
}

void BaseNode::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}

void BaseNode::emitWrite(json msg)
{
    const std::string& data = m_converter.toString(msg);
    emitLog(LogLevel::Debug, "writeMessage " + msg.dump());
    if(m_writeFunc) {
        m_writeFunc(data);
    } else {
        emitLog(LogLevel::Warning, "no writer set, can not write");
    }
}

void BaseNode::onLog(WriteLogFunc func)
{
    m_logFunc = func;
}

void BaseNode::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}


void BaseNode::handleMessage(std::string data)
{
    const json& j = m_converter.fromString(data);
    m_protocol.handleMessage(j);
}

std::string BaseNode::name() const
{
    return m_name;
}



} } // ApiGear::ObjectLink
