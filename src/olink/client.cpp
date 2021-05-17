#include "client.h"

namespace ApiGear { namespace ObjectLink {

ClientIO::ClientIO(std::string name)
    : m_writeFunc(nullptr)
    , m_logFunc(nullptr)
    , m_converter(MessageFormat::JSON)
    , m_protocol(this)
    , m_nextId(0)
    , m_name(name)
    , m_registry(name)
{
}

void ClientIO::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}


void ClientIO::invoke(std::string name, json args, InvokeReplyFunc func)
{
    int requestId = nextId();
    m_invokesPending[requestId] = func;
    json msg = Protocol::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int ClientIO::nextId()
{
    m_nextId++;
    return m_nextId;
}


void ClientIO::setProperty(std::string name, json value)
{
    json msg = Protocol::setPropertyMessage(name, value);
    emitWrite(msg);
}

void ClientIO::emitWrite(json msg)
{
    const std::string& data = m_converter.toString(msg);
    emitLog(LogLevel::Debug, "writeMessage " + msg.dump());
    if(m_writeFunc) {
        m_writeFunc(data);
    } else {
        emitLog(LogLevel::Warning, "no writer set, can not write");
    }
}


void ClientIO::onLog(LogWriterFunc func)
{
    m_logFunc = func;
    m_registry.onLog(func);
}

void ClientIO::handleMessage(std::string data)
{
    const json& j = m_converter.fromString(data);
    m_protocol.handleMessage(j);
}

void ClientIO::link(std::string name)
{
    emitWrite(Protocol::linkMessage(name));
}

void ClientIO::unlink(std::string name)
{
    emitWrite(Protocol::unlinkMessage(name));
}

std::string ClientIO::name() const
{
    return m_name;
}

SinkRegistry &ClientIO::registry()
{
    return m_registry;
}

void ClientIO::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}

void ClientIO::handleInit(std::string name, json props)
{
    emitLog(LogLevel::Info, "handle init" +  name + props.dump());
    ISink *s = registry().objectSink(name);
    if(s) {
        s->onInit(name, props, this);
    }
}


void ClientIO::handlePropertyChange(std::string name, json value)
{
    ISink *l = registry().objectSink(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void ClientIO::handleInvokeReply(int requestId, std::string name, json value)
{
    emitLog(LogLevel::Info, "handle invoke reply " + std::to_string(requestId) + name + value.dump());
    if(m_invokesPending.count(requestId) == 1) {
        const InvokeReplyFunc& func = m_invokesPending[requestId];
        if(func) {
            const InvokeReplyArg arg{name, value};
            func(arg);
        }
        m_invokesPending.erase(requestId);
    } else {
        emitLog(LogLevel::Warning, "no pending invoke " + name + std::to_string(requestId));
    }
}

void ClientIO::handleSignal(std::string name, json args)
{
    ISink *s = registry().objectSink(name);
    if(s) {
        s->onSignal(name, args);
    }
}

void ClientIO::handleError(int msgType, int requestId, std::string error)
{
    emitLog(LogLevel::Info, "handleError not implemented " + std::to_string(msgType) + std::to_string(requestId) + error);
}


} } // ApiGear::ObjectLink
