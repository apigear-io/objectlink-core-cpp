#include "clientregistry.h"
#include "iobjectsink.h"
#include "clientnode.h"


namespace ApiGear {
namespace ObjectLink {

void ClientRegistry::setNode(ClientNode& node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.setNode: " + objectId);
    auto& foundEntry = entry(objectId);
    if (foundEntry.node == nullptr){
        foundEntry.node = &node;
    } else {
        emitLog(LogLevel::Warning, "Trying to set a client node for " + objectId + " but node is already set. Node was NOT changed.");
    }
}

void ClientRegistry::unsetNode(ClientNode& node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.unsetNode: " + objectId);
    auto foundEntry = m_entries.find(objectId);
    if (foundEntry != m_entries.end() &&  foundEntry->second.node  == &node){
        foundEntry->second.node = nullptr;
    }
}

void ClientRegistry::addSink(IObjectSink& sink)
{
    const auto& objectId = sink.olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.addSink: " + objectId);
    auto& entryForObject = entry(objectId);
    if (entryForObject.sink == nullptr){
        entryForObject.sink = &sink;
    } else {
        emitLog(LogLevel::Warning, "Trying to add object for " + objectId + " but object for this id is already registerd. New object NOT added.");
    }
}

void ClientRegistry::removeSink(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.removeSink: " + objectId);
    removeEntry(objectId);
}

IObjectSink* ClientRegistry::getSink(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getSink: " + objectId);
    return entry(objectId).sink;
}

std::vector<std::string> ClientRegistry::getObjectIds(ClientNode& node)
{
    std::vector<std::string> sinks;
    for (auto& entry : m_entries) {
        if (entry.second.node == &node) {
            sinks.push_back(entry.first);
        }
    }
    return sinks;
}

ClientNode* ClientRegistry::getNode(const std::string& objectId)
{
    emitLog(LogLevel::Info, "ClientRegistry.getNode: " + objectId);
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
