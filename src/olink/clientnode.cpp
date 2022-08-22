#include "clientnode.h"
#include "clientregistry.h"
#include "iobjectsink.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


ClientNode::ClientNode(ClientRegistry& registry)
    : BaseNode()
    , m_nextRequestId(0)
    , m_registry(registry)
{
    m_registry.attachClientNode(*this);
}

ClientNode::~ClientNode()
{
    connectionToBeReleased();
}

void ClientNode::linkNode(const std::string& interfaceId)
{
    m_registry.linkClientNode(interfaceId, *this);
}

void ClientNode::connectionEstablished()
{
    auto names = m_registry.getObjects(*this);
    for (auto& objectName : names) {
        linkRemote(objectName);
    }
}

void ClientNode::connectionToBeReleased()
{
    auto names = m_registry.getObjects(*this);
    for (auto& name : names) {
        auto sink = m_registry.getObjectSink(name);
        if (sink){
            sink->olinkOnRelease();
        }
    }
    m_registry.detachClientNode(*this);
}

void ClientNode::linkRemote(const std::string& interfaceId)
{
    emitLog(LogLevel::Info, "ClientNode.linkRemote: " + interfaceId);
    emitWrite(Protocol::linkMessage(interfaceId));
}

void ClientNode::unlinkRemote(const std::string& interfaceId)
{
    emitLog(LogLevel::Info, "ClientNode.unlinkRemote: " + interfaceId);
    emitWrite(Protocol::unlinkMessage(interfaceId));
}

void ClientNode::invokeRemote(const std::string& methodId, nlohmann::json args, InvokeReplyFunc func)
{
    emitLog(LogLevel::Info, "ClientNode.invokeRemote: " + methodId);
    int requestId = nextRequestId();
    m_invokesPending[requestId] = func;
    nlohmann::json msg = Protocol::invokeMessage(requestId, methodId, args);
    emitWrite(msg);
}

void ClientNode::setRemoteProperty(const std::string& propertyId, nlohmann::json value)
{
    emitLog(LogLevel::Info, "ClientNode.setRemoteProperty: " + propertyId);
    nlohmann::json msg = Protocol::setPropertyMessage(propertyId, value);
    emitWrite(msg);
}

ClientRegistry& ClientNode::registry()
{
    return m_registry;
}

void ClientNode::handleInit(const std::string& interfaceId, const nlohmann::json& props)
{
    emitLog(LogLevel::Info, "ClientNode.handleInit: " + interfaceId + props.dump());
    auto sink = m_registry.getObjectSink(interfaceId);
    if(sink) {
        sink->olinkOnInit(interfaceId, props, this);
    }
}

void ClientNode::handlePropertyChange(const std::string& propertyId, const nlohmann::json& value)
{
    emitLog(LogLevel::Info, "ClientNode.handlePropertyChange: " + propertyId + value.dump());
    auto sink = m_registry.getObjectSink(Name::getInterfaceId(propertyId));
    if(sink){
        sink->olinkOnPropertyChanged(propertyId, value);
    }
}

void ClientNode::handleInvokeReply(int requestId, const std::string& methodId, const nlohmann::json& value)
{
    emitLog(LogLevel::Info, "ClientNode.handleInvokeReply: " + methodId + value.dump());
    if(m_invokesPending.count(requestId) == 1) {
        const InvokeReplyFunc& func = m_invokesPending[requestId];
        if(func) {
            const InvokeReplyArg arg{ methodId, value};
            func(arg);
        }
        m_invokesPending.erase(requestId);
    } else {
        emitLog(LogLevel::Warning, "no pending invoke " + methodId + std::to_string(requestId));
    }
}

void ClientNode::handleSignal(const std::string& signalId, const nlohmann::json& args)
{
    emitLog(LogLevel::Info, "ClientNode.handleSignal: " + signalId);
    auto sink = m_registry.getObjectSink(Name::getInterfaceId(signalId));
    if(sink) {
        sink->olinkOnSignal(signalId, args);
    }
}

void ClientNode::handleError(int msgType, int requestId, const std::string& error)
{
    emitLog(LogLevel::Info, "ClientNode.handleError: " + std::to_string(msgType) + std::to_string(requestId) + error);
}

int ClientNode::nextRequestId()
{
    m_nextRequestId++;
    if (m_nextRequestId < 0){
        m_nextRequestId = 0;
    }
    return m_nextRequestId;
}

} } // ApiGear::ObjectLink
