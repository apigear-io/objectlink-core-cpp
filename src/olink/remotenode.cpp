// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include "remotenode.h"
#include "remoteregistry.h"
#include "iobjectsource.h"
#include "core/protocol.h"

namespace ApiGear { namespace ObjectLink {

RemoteNode::RemoteNode(RemoteRegistry& registry)
    : m_registry(registry)
{
}

void RemoteNode::setNodeId(unsigned long id){
    m_nodeId = id;
}

std::shared_ptr<RemoteNode> RemoteNode::createRemoteNode(RemoteRegistry& registry)
{
    auto node = std::shared_ptr<RemoteNode>(new RemoteNode(registry));
    node->setNodeId(registry.registerNode(node));
    return node;
}

RemoteNode::~RemoteNode()
{
    m_registry.unregisterNode(m_nodeId);
}

void RemoteNode::handleLink(const std::string& objectId)
{
    static const std::string handleLinkLog = "handleLink name: ";
    emitLog(LogLevel::Info, handleLinkLog, objectId);
    auto source = m_registry.getSource(objectId).lock();
    if(source) {
        m_registry.addNodeForSource(m_nodeId, objectId);
        source->olinkLinked(objectId, this);
        nlohmann::json props = source->olinkCollectProperties();
        emitWrite(Protocol::initMessage(objectId, props));
    } else {
        static const std::string noLinkToSourceLog = "no source to link: ";
        emitLog(LogLevel::Warning, noLinkToSourceLog, objectId);
    }

}

void RemoteNode::handleUnlink(const std::string& objectId)
{
    auto source = m_registry.getSource(objectId).lock();
    if(source) {
        source->olinkUnlinked(objectId);
        m_registry.removeNodeFromSource(m_nodeId, objectId);
    }
}

void RemoteNode::handleSetProperty(const std::string& propertyId, const nlohmann::json& value)
{
    auto objectId = ApiGear::ObjectLink::Name::getObjectId(propertyId);
    auto source = m_registry.getSource(objectId).lock();
    if(source) {
        source->olinkSetProperty(propertyId, value);
    }
}

void RemoteNode::handleInvoke(unsigned int requestId, const std::string& methodId, const nlohmann::json& args)
{
    auto objectId = ApiGear::ObjectLink::Name::getObjectId(methodId);
    auto source = m_registry.getSource(objectId).lock();
    if(source) {
        nlohmann::json value = source->olinkInvoke(methodId, args);
        emitWrite(Protocol::invokeReplyMessage(requestId, methodId, value));
    }
}

void RemoteNode::notifyPropertyChange(const std::string& propertyId, const nlohmann::json& value)
{
    emitWrite(Protocol::propertyChangeMessage(propertyId, value));
}

void RemoteNode::notifySignal(const std::string& signalId, const nlohmann::json& args)
{
    emitWrite(Protocol::signalMessage(signalId, args));
}

RemoteRegistry& RemoteNode::registry()
{
    return m_registry;
}

unsigned long RemoteNode::getNodeId() const {
    return m_nodeId;
}

} } // Apigear::ObjectLink
