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
}

ClientNode::~ClientNode()
{
    unlinkRemoteForAllSinks();
    auto objects = m_registry.getObjectIds(*this);
    for (auto& objectId : objects) {
        m_registry.unsetNode(*this, objectId);
    }
}

void ClientNode::linkRemoteForAllSinks()
{
    auto names = m_registry.getObjectIds(*this);
    for (auto& objectName : names) {
        linkRemote(objectName);
    }
}

void ClientNode::unlinkRemoteForAllSinks()
{
    auto objects = m_registry.getObjectIds(*this);
    for (auto& objectId : objects) {
        unlinkRemote(objectId);
    }
}

void ClientNode::linkRemote(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientNode.linkRemote: " + objectId);
    emitWrite(Protocol::linkMessage(objectId));
}

void ClientNode::unlinkRemote(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientNode.unlinkRemote: " + objectId);
    auto sink = m_registry.getSink(objectId);
    if (sink){
        sink->olinkOnRelease();
    }
    emitWrite(Protocol::unlinkMessage(objectId));
}

void ClientNode::invokeRemote(const std::string& methodId, const nlohmann::json& args, InvokeReplyFunc func)
{
    emitLog(LogLevel::Info, "ClientNode.invokeRemote: " + methodId);
    int requestId = nextRequestId();
    m_invokesPending[requestId] = func;
    nlohmann::json msg = Protocol::invokeMessage(requestId, methodId, args);
    emitWrite(msg);
}

void ClientNode::setRemoteProperty(const std::string& propertyId, const nlohmann::json& value)
{
    emitLog(LogLevel::Info, "ClientNode.setRemoteProperty: " + propertyId);
    nlohmann::json msg = Protocol::setPropertyMessage(propertyId, value);
    emitWrite(msg);
}

ClientRegistry& ClientNode::registry()
{
    return m_registry;
}

void ClientNode::handleInit(const std::string& objectId, const nlohmann::json& props)
{
    emitLog(LogLevel::Info, "ClientNode.handleInit: " + objectId + props.dump());
    auto sink = m_registry.getSink(objectId);
    if(sink) {
        sink->olinkOnInit(objectId, props, this);
    }
    else {
        emitLog(LogLevel::Warning, "No sink found for id" + objectId);
    }
}

void ClientNode::handlePropertyChange(const std::string& propertyId, const nlohmann::json& value)
{
    emitLog(LogLevel::Info, "ClientNode.handlePropertyChange: " + propertyId + value.dump());
    auto sink = m_registry.getSink(Name::getObjectId(propertyId));
    if(sink){
        sink->olinkOnPropertyChanged(propertyId, value);
    }
    else {
        emitLog(LogLevel::Warning, "No sink found for id" + Name::getObjectId(propertyId));
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
    auto sink = m_registry.getSink(Name::getObjectId(signalId));
    if(sink) {
        sink->olinkOnSignal(signalId, args);
    } else {
        emitLog(LogLevel::Warning, "No sink found for id" + Name::getObjectId(signalId));
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
