#include "clientregistry.h"
#include "iobjectsink.h"
#include "clientnode.h"


namespace ApiGear {
namespace ObjectLink {


ClientRegistry::ClientRegistry()
    : Base()
{
}

void ClientRegistry::attachClientNode(ClientNode& node)
{
    emitLog(LogLevel::Info, "ClientRegistry.attachClientNode");
    // nothing
}

void ClientRegistry::detachClientNode(ClientNode& node)
{
    emitLog(LogLevel::Info, "ClientRegistry.detachClientNode");
    for (auto& link : m_entries) {
        if (link.second.node == &node) {
            link.second.node = nullptr;
        }
    }
}

void ClientRegistry::linkClientNode(const std::string& interfaceId, ClientNode& node)
{
    emitLog(LogLevel::Info, "ClientRegistry.linkClientNode: " + interfaceId);
    auto& foundEntry = entry(interfaceId);
    if (foundEntry.node == nullptr)
    {
        foundEntry.node = &node;
    }
    else
    {
        emitLog(LogLevel::Warning, "Trying to add client node for " + interfaceId + " but node already added. Node NOT added.");
    }
}

void ClientRegistry::unlinkClientNode(const std::string& interfaceId, ClientNode& node)
{
    emitLog(LogLevel::Info, "ClientRegistry.unlinkClientNode: " + interfaceId);
    auto foundEntry = m_entries.find(interfaceId);
    if (foundEntry != m_entries.end() &&
        foundEntry->second.node  == &node)
    {
        foundEntry->second.node == nullptr;
    }
}

void ClientRegistry::addObjectSink(IObjectSink& sink)
{
    const auto& interfaceId = sink.olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.addObjectSink: " + interfaceId);
    auto& foundEntry = entry(interfaceId);
    if (foundEntry.sink == nullptr)
    {
        foundEntry.sink = &sink;
    }
    else
    {
        emitLog(LogLevel::Warning, "Trying to add client node for " + interfaceId + " but node already added. Node NOT added.");
    }
}

void ClientRegistry::removeObjectSink(IObjectSink& sink)
{
    const auto& interfaceId = sink.olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.removeObjectSink: " + interfaceId);
    removeEntry(interfaceId);
}

IObjectSink* ClientRegistry::getObjectSink(const std::string& interfaceId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getObjectSink: " + interfaceId);
    return entry(interfaceId).sink;
}

std::vector<std::string> ClientRegistry::getObjects(ClientNode& node)
{
    std::vector<std::string> sinks;
    for (auto& entry : m_entries) {
        if (entry.second.node == &node && entry.second.sink) {
            sinks.push_back(entry.first);
        }
    }
    return sinks;
}

ClientNode* ClientRegistry::getClientNode(const std::string& interfaceId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getClientNode: " + interfaceId);
    return entry(interfaceId).node;
}

SinkToClientEntry& ClientRegistry::entry(const std::string& interfaceId) {
    if (m_entries.count(interfaceId) == 0) {
        m_entries[interfaceId] = SinkToClientEntry();
    }
    return m_entries[interfaceId];
}

void ClientRegistry::removeEntry(const std::string& interfaceId)
{
    if (m_entries.count(interfaceId) > 0) {
        m_entries.erase(interfaceId);
    }
}

}} //namespace ApiGear::ObjectLink