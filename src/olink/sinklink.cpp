#include "sinklink.h"

#include "olink/sinknode.h"

namespace ApiGear { namespace ObjectLink {

SinkLink::SinkLink(std::string name)
    : ObjectLink(name)
    , m_nextId(0)
{
}



void SinkLink::invoke(std::string name, json args, InvokeReplyFunc func)
{
    int requestId = nextId();
    m_invokesPending[requestId] = func;
    json msg = Protocol::invokeMessage(requestId, name, args);
    emitWrite(msg);
}

int SinkLink::nextId()
{
    m_nextId++;
    return m_nextId;
}


void SinkLink::setProperty(std::string name, json value)
{
    json msg = Protocol::setPropertyMessage(name, value);
    emitWrite(msg);
}


void SinkLink::link(std::string name)
{
    emitWrite(Protocol::linkMessage(name));
}

void SinkLink::unlink(std::string name)
{
    emitWrite(Protocol::unlinkMessage(name));
}

SinkNode *SinkLink::sinkNode()
{
    SinkNode* node = SinkNodeManager::get().getSinkNode(nodeName());
    if(!node) {
        emitLog(LogLevel::Warning, "unknown node: " + nodeName());
        return nullptr;
    }
    return node;
}

void SinkLink::handleInit(std::string name, json props)
{
    emitLog(LogLevel::Info, "handle init" +  name + props.dump());
    IObjectSink *s = sinkNode()->getObjectSink(name);
    if(s) {
        s->onInit(name, props, this);
    }
}


void SinkLink::handlePropertyChange(std::string name, json value)
{
    IObjectSink *l = sinkNode()->getObjectSink(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void SinkLink::handleInvokeReply(int requestId, std::string name, json value)
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

void SinkLink::handleSignal(std::string name, json args)
{
    IObjectSink *s = sinkNode()->getObjectSink(name);
    if(s) {
        s->onSignal(name, args);
    }
}

void SinkLink::handleError(int msgType, int requestId, std::string error)
{
    emitLog(LogLevel::Info, "handleError not implemented " + std::to_string(msgType) + std::to_string(requestId) + error);
}


} } // ApiGear::ObjectLink
