#include "clientnode.h"
#include "clientregistry.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


ClientNode::ClientNode(ClientRegistry& registry)
    : BaseNode()
    , m_nextRequestId(0)
    , m_registry(&registry)
{
    m_registry->attachClientNode(this);
}

ClientNode::~ClientNode()
{
    m_registry->detachClientNode(this);
}

void ClientNode::linkNode(std::string name)
{
    m_registry->linkClientNode(name, this);
}

void ClientNode::unlinkNode(std::string name)
{
    m_registry->unlinkClientNode(name, this);
}



void ClientNode::invokeRemote(std::string name, nlohmann::json args, InvokeReplyFunc func)
{
    emitLog(LogLevel::Info, "ClientNode.invokeRemote: " + name);
    int requestId = nextRequestId();
    m_invokesPending[requestId] = func;
    nlohmann::json msg = Protocol::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int ClientNode::nextRequestId()
{
    m_nextRequestId++;
    return m_nextRequestId;
}


void ClientNode::setRemoteProperty(std::string name, nlohmann::json value)
{
    emitLog(LogLevel::Info, "ClientNode.setRemoteProperty: " + name);
    nlohmann::json msg = Protocol::setPropertyMessage(name, value);
    emitWrite(msg);
}

ClientRegistry &ClientNode::registry()
{
    return *m_registry;
}

ClientNode *ClientNode::addObjectSink(IObjectSink *sink)
{
    return m_registry->addObjectSink(sink);
}

void ClientNode::removeObjectSink(IObjectSink *sink)
{
    m_registry->removeObjectSink(sink);
}

IObjectSink *ClientNode::getObjectSink(std::string name)
{
    return m_registry->getObjectSink(name);
}


void ClientNode::linkRemote(std::string name)
{
    emitLog(LogLevel::Info, "ClientNode.linkRemote: " + name);
    emitWrite(Protocol::linkMessage(name));
}

void ClientNode::unlinkRemote(std::string name)
{
    emitLog(LogLevel::Info, "ClientNode.unlinkRemote: " + name);
    emitWrite(Protocol::unlinkMessage(name));
}

void ClientNode::handleInit(std::string name, nlohmann::json props)
{
    emitLog(LogLevel::Info, "ClientNode.handleInit: " + name + props.dump());
    IObjectSink *s = getObjectSink(name);
    if(s) {
        s->olinkOnInit(name, props, this);
    }
}


void ClientNode::handlePropertyChange(std::string name, nlohmann::json value)
{
    emitLog(LogLevel::Info, "ClientNode.handlePropertyChange: " + name + value.dump());
    IObjectSink *s = getObjectSink(name);
    if(s) {
        s->olinkOnPropertyChanged(name, value);
    }
}

void ClientNode::handleInvokeReply(int requestId, std::string name, nlohmann::json value)
{
    emitLog(LogLevel::Info, "ClientNode.handleInvokeReply: " + name + value.dump());
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

void ClientNode::handleSignal(std::string name, nlohmann::json args)
{
    emitLog(LogLevel::Info, "ClientNode.handleSignal: " + name);
    IObjectSink *s = getObjectSink(name);
    if(s) {
        s->olinkOnSignal(name, args);
    }
}

void ClientNode::handleError(int msgType, int requestId, std::string error)
{
    emitLog(LogLevel::Info, "ClientNode.handleError: " + std::to_string(msgType) + std::to_string(requestId) + error);
}




} } // ApiGear::ObjectLink
