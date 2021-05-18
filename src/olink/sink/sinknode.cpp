#include "sinknode.h"
#include "sinkregistry.h"

namespace ApiGear { namespace ObjectLink {


ObjectSinkNode::ObjectSinkNode(std::string name)
    : BaseNode(name)
    , m_nextId(0)
{
}



void ObjectSinkNode::invoke(std::string name, json args, InvokeReplyFunc func)
{
    int requestId = nextId();
    m_invokesPending[requestId] = func;
    json msg = ObjectLinkProtocol::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int ObjectSinkNode::nextId()
{
    m_nextId++;
    return m_nextId;
}


void ObjectSinkNode::setProperty(std::string name, json value)
{
    json msg = ObjectLinkProtocol::setPropertyMessage(name, value);
    emitWrite(msg);
}


void ObjectSinkNode::link(std::string name)
{
    emitWrite(ObjectLinkProtocol::linkMessage(name));
}

void ObjectSinkNode::unlink(std::string name)
{
    emitWrite(ObjectLinkProtocol::unlinkMessage(name));
}

ObjectSinkRegistry *ObjectSinkNode::registry()
{
    ObjectSinkRegistry* node = ObjectSinkRegistry::registry(name());
    if(!node) {
        emitLog(LogLevel::Warning, "unknown node: " + name());
        return nullptr;
    }
    return node;
}

void ObjectSinkNode::handleInit(std::string name, json props)
{
    emitLog(LogLevel::Info, "handle init" +  name + props.dump());
    IObjectSink *s = registry()->getObjectSink(name);
    if(s) {
        s->onInit(name, props, this);
    }
}


void ObjectSinkNode::handlePropertyChange(std::string name, json value)
{
    IObjectSink *l = registry()->getObjectSink(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void ObjectSinkNode::handleInvokeReply(int requestId, std::string name, json value)
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

void ObjectSinkNode::handleSignal(std::string name, json args)
{
    IObjectSink *s = registry()->getObjectSink(name);
    if(s) {
        s->onSignal(name, args);
    }
}

void ObjectSinkNode::handleError(int msgType, int requestId, std::string error)
{
    emitLog(LogLevel::Info, "handleError not implemented " + std::to_string(msgType) + std::to_string(requestId) + error);
}


} } // ApiGear::ObjectLink
