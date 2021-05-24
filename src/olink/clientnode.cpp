#include "clientnode.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


// ********************************************************************
// ClientRegistry
// ********************************************************************

ClientRegistry::ClientRegistry()
    : Base()
{
}

ClientRegistry::~ClientRegistry()
{
}

ClientRegistry &ClientRegistry::get()
{
    static ClientRegistry r;
    return r;
}

void ClientRegistry::attachClientNode(ClientNode *node)
{
    emitLog(LogLevel::Info, "ClientRegistry.attachClientNode");
    // nothing
}

void ClientRegistry::detachClientNode(ClientNode *node)
{
    emitLog(LogLevel::Info, "ClientRegistry.detachClientNode");
    for(auto & link: m_entries) {
        if(link.second.node == node) {
            link.second.node = nullptr;
        }
    }
}

void ClientRegistry::linkClientNode(std::string name, ClientNode *node)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ClientRegistry.linkClientNode: " + resource);
    if(!hasEntry(resource)) {
        initEntry(resource);
    }
    if(!m_entries[resource].node) {
        m_entries[resource].node = node;
    } else {
        emitLog(LogLevel::Info, "link node failed: sink has already a node" + resource);
    }
}

void ClientRegistry::unlinkClientNode(std::string name, ClientNode *node)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ClientRegistry.unlinkClientNode: " + resource);
    if(hasEntry(resource)) {
        if(m_entries[resource].node == node) {
            m_entries[resource].node = nullptr;
        } else {
            emitLog(LogLevel::Info, "unlink node failed, not the same node" + resource);
        }
    }
}

ClientNode* ClientRegistry::addObjectSink(IObjectSink *sink)
{
    std::string resource = Name::resourceFromName(sink->olinkObjectName());
    emitLog(LogLevel::Info, "ClientRegistry.addObjectSink: " + resource);
    if(!hasEntry(resource)) {
        initEntry(resource);
    }
    emitLog(LogLevel::Info, "link sink: " + resource);
    if(!m_entries[resource].sink) {
        m_entries[resource].sink = sink;
    } else {
        emitLog(LogLevel::Info, "update link: " + resource);
        if(!entry(resource).sink) {
            m_entries[resource].sink = sink;
        } else {
        emitLog(LogLevel::Info, "add object sink failed: sink already added" + resource);
        }
    }
    return m_entries[resource].node;
}

void ClientRegistry::removeObjectSink(IObjectSink *sink)
{
    std::string resource = Name::resourceFromName(sink->olinkObjectName());
    emitLog(LogLevel::Info, "ClientRegistry.removeObjectSink: " + resource);
    if(hasEntry(resource)) {
        m_entries.erase(resource);
    } else {
        emitLog(LogLevel::Info, "remove object sink failed: no sink to remove" + resource);
    }
}

IObjectSink *ClientRegistry::getObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ClientRegistry.getObjectSink: " + resource);
    if(hasEntry(resource)) {
        IObjectSink *sink = m_entries[resource].sink;
        if(!sink) {
            emitLog(LogLevel::Info, "no sink attached " + resource);
        }
        return sink;
    } else {
        emitLog(LogLevel::Info, "no object sink " + resource);
    }
    return nullptr;

}

ClientNode *ClientRegistry::getClientNode(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ClientRegistry.getClientNode: " + resource);
    if(hasEntry(resource)) {
        ClientNode *node = m_entries[resource].node;
        if(!node) {
            emitLog(LogLevel::Info, "no node attached " + resource);
        }
        return node;
    } else {
        emitLog(LogLevel::Info, "no object sink " + resource);
    }
    return nullptr;
}

ClientNode *ClientRegistry::getClientNode(IObjectSink *sink) {
    return getClientNode(sink->olinkObjectName());
}

SinkToClientEntry& ClientRegistry::entry(std::string name) {
    std::string resource = Name::resourceFromName(name);
    return m_entries[resource];
}

bool ClientRegistry::hasEntry(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    return m_entries.count(resource) != 0;
}

void ClientRegistry::initEntry(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ensure link: " + resource);
    if(m_entries.count(resource) == 0) {
        m_entries[resource] = SinkToClientEntry();
    }
}

// ********************************************************************
// IClientNode
// ********************************************************************





IClientNode::~IClientNode()
{

}

// ********************************************************************
// IObjectSink
// ********************************************************************


IObjectSink::~IObjectSink()
{

}


// ********************************************************************
// ClientNode
// ********************************************************************

ClientNode::ClientNode()
    : BaseNode()
    , m_nextRequestId(0)
{
    registry().attachClientNode(this);
}

ClientNode::~ClientNode()
{
    registry().detachClientNode(this);
}

void ClientNode::linkNode(std::string name)
{
    registry().linkClientNode(name, this);
}

void ClientNode::unlinkNode(std::string name)
{
    registry().unlinkClientNode(name, this);
}



void ClientNode::invokeRemote(std::string name, json args, InvokeReplyFunc func)
{
    emitLog(LogLevel::Info, "ClientNode.invokeRemote: " + name);
    int requestId = nextRequestId();
    m_invokesPending[requestId] = func;
    json msg = Protocol::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int ClientNode::nextRequestId()
{
    m_nextRequestId++;
    return m_nextRequestId;
}


void ClientNode::setRemoteProperty(std::string name, json value)
{
    emitLog(LogLevel::Info, "ClientNode.setRemoteProperty: " + name);
    json msg = Protocol::setPropertyMessage(name, value);
    emitWrite(msg);
}

ClientRegistry &ClientNode::registry()
{
    return ClientRegistry::get();
}

ClientNode *ClientNode::addObjectSink(IObjectSink *sink)
{
    return ClientRegistry::get().addObjectSink(sink);
}

void ClientNode::removeObjectSink(IObjectSink *sink)
{
    ClientRegistry::get().removeObjectSink(sink);
}

IObjectSink *ClientNode::getObjectSink(std::string name)
{
    return registry().getObjectSink(name);
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

void ClientNode::handleInit(std::string name, json props)
{
    emitLog(LogLevel::Info, "ClientNode.handleInit: " + name + props.dump());
    IObjectSink *s = getObjectSink(name);
    if(s) {
        s->olinkOnInit(name, props, this);
    }
}


void ClientNode::handlePropertyChange(std::string name, json value)
{
    emitLog(LogLevel::Info, "ClientNode.handlePropertyChange: " + name + value.dump());
    IObjectSink *s = getObjectSink(name);
    if(s) {
        s->olinkOnPropertyChanged(name, value);
    }
}

void ClientNode::handleInvokeReply(int requestId, std::string name, json value)
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

void ClientNode::handleSignal(std::string name, json args)
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
