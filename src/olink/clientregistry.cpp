#include "clientregistry.h"
#include "iobjectsink.h"
#include "clientnode.h"


namespace ApiGear {
namespace ObjectLink {


ClientRegistry::ClientRegistry()
    : Base()
{
}

void ClientRegistry::linkToObject(ClientNode& node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.linkToObject: " + objectId);
    auto& foundEntry = entry(objectId);
    if (foundEntry.node == nullptr)
    {
        foundEntry.node = &node;
    }
    else
    {
        emitLog(LogLevel::Warning, "Trying to add client node for " + objectId + " but node already added. Node NOT added.");
    }
}

void ClientRegistry::unlinkFromObject(ClientNode& node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.unlinkFromObject: " + objectId);
    auto foundEntry = m_entries.find(objectId);
    if (foundEntry != m_entries.end() &&
        foundEntry->second.node  == &node)
    {
        foundEntry->second.node == nullptr;
    }
}

void ClientRegistry::addObject(IObjectSink& sink)
{
    const auto& objectId = sink.olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.addObject: " + objectId);
    auto& foundEntry = entry(objectId);
    if (foundEntry.sink == nullptr)
    {
        foundEntry.sink = &sink;
    }
    else
    {
        emitLog(LogLevel::Warning, "Trying to add client node for " + objectId + " but node already added. Node NOT added.");
    }
}

void ClientRegistry::removeObject(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.removeObject: " + objectId);
    removeEntry(objectId);
}

IObjectSink* ClientRegistry::getObjectSink(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getObjectSink: " + objectId);
    return entry(objectId).sink;
}

std::vector<std::string> ClientRegistry::getObjectsId(ClientNode& node)
{
    std::vector<std::string> sinks;
    for (auto& entry : m_entries) {
        if (entry.second.node == &node && entry.second.sink) {
            sinks.push_back(entry.first);
        }
    }
    return sinks;
}

ClientNode* ClientRegistry::getClientNode(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getClientNode: " + objectId);
    return entry(objectId).node;
}

ClientRegistry::SinkToClientEntry& ClientRegistry::entry(const std::string& objectId) {
    if (m_entries.count(objectId) == 0) {
        m_entries[objectId] = SinkToClientEntry();
    }
    return m_entries[objectId];
}

void ClientRegistry::removeEntry(const std::string& objectId)
{
    if (m_entries.count(objectId) > 0) {
        m_entries.erase(objectId);
    }
}

}} //namespace ApiGear::ObjectLink