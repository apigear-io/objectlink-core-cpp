#include "client.h"

namespace ApiGear { namespace ObjectLink {

Client::Client(std::string name)
    : m_writeFunc(nullptr)
    , m_logFunc(nullptr)
    , m_converter(MessageFormat::JSON)
    , m_messages(this)
    , m_nextId(0)
    , m_name(name)
    , m_registry(name)
{
}

void Client::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}


void Client::invoke(std::string name, json args, InvokeReplyFunc func)
{
    int requestId = nextId();
    m_invokesPending[requestId] = func;
    json msg = Messages::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int Client::nextId()
{
    m_nextId++;
    return m_nextId;
}


void Client::setProperty(std::string name, json value)
{
    json msg = Messages::setPropertyMessage(name, value);
    emitWrite(msg);
}

void Client::emitWrite(json msg)
{
    const std::string& data = m_converter.toString(msg);
    emitLog(LogLevel::Debug, "writeMessage " + msg.dump());
    if(m_writeFunc) {
        m_writeFunc(data);
    } else {
        emitLog(LogLevel::Warning, "no writer set, can not write");
    }
}


void Client::onLog(LogWriterFunc func)
{
    m_logFunc = func;
}

void Client::handleMessage(std::string data)
{
    const json& j = m_converter.fromString(data);
    m_messages.handleMessage(j);
}

std::string Client::name() const
{
    return m_name;
}

SinkRegistry &Client::registry()
{
    return m_registry;
}

void Client::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}

void Client::handleLink(std::string name)
{
    emitLog(LogLevel::Info, "handleLink not implemented");
}

void Client::handleUnlink(std::string name)
{
    emitLog(LogLevel::Info, "handleUnlink not implemented");
}

void Client::handleInit(std::string name, json props)
{
    emitLog(LogLevel::Info, "handle init" +  name + props.dump());
    IObjectLinkSink *s = registry().objectSink(name);
    if(s) {
        s->onInit(name, props, this);
    }
}

void Client::handleSetProperty(std::string name, json value)
{
    emitLog(LogLevel::Info, "handleUnlink not implemented");
 }

void Client::handlePropertyChange(std::string name, json value)
{
    IObjectLinkSink *l = registry().objectSink(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void Client::handleInvoke(int requestId, std::string name, json args)
{
    emitLog(LogLevel::Info, "handleInvoke not implemented");
}

void Client::handleInvokeReply(int requestId, std::string name, json value)
{
    emitLog(LogLevel::Info, "handle invoke reply " + std::to_string(requestId) + name + value.dump());
    if(m_invokesPending.count(requestId) == 1) {
        const InvokeReplyFunc& func = m_invokesPending[requestId];
        const InvokeReplyArg arg{name, value};
        func(arg);
        m_invokesPending.erase(requestId);
    } else {
        emitLog(LogLevel::Warning, "no pending invoke " + name + std::to_string(requestId));
    }
}

void Client::handleSignal(std::string name, json args)
{
    IObjectLinkSink *s = registry().objectSink(name);
    if(s) {
        s->onSignal(name, args);
    }
}

void Client::handleError(int msgType, int requestId, std::string error)
{
    emitLog(LogLevel::Info, "handleError not implemented " + std::to_string(msgType) + std::to_string(requestId) + error);
}


} } // ApiGear::ObjectLink
