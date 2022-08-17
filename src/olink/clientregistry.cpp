#include "clientregistry.h"
#include "iobjectsink.h"
#include "clientnode.h"


namespace ApiGear {
namespace ObjectLink {


ClientRegistry::ClientRegistry()
    : Base()
{
}

void ClientRegistry::attachClientNode(ClientNode* node)
{
    emitLog(LogLevel::Info, "ClientRegistry.attachClientNode");
    // nothing
}

void ClientRegistry::detachClientNode(ClientNode* node)
{
    emitLog(LogLevel::Info, "ClientRegistry.detachClientNode");
    for (auto& link : m_entries) {
        if (link.second.node == node) {
            link.second.node = nullptr;
        }
    }
}

void ClientRegistry::linkClientNode(std::string name, ClientNode* node)
{
    emitLog(LogLevel::Info, "ClientRegistry.linkClientNode: " + name);
    entry(name).node = node;
}

void ClientRegistry::unlinkClientNode(std::string name, ClientNode* node)
{
    emitLog(LogLevel::Info, "ClientRegistry.unlinkClientNode: " + name);
    entry(name).node = nullptr;
}

ClientNode* ClientRegistry::addObjectSink(IObjectSink* sink)
{
    std::string name = sink->olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.addObjectSink: " + name);
    SinkToClientEntry& e = entry(name);
    e.sink = sink;
    return e.node;
}

void ClientRegistry::removeObjectSink(IObjectSink* sink)
{
    std::string name = sink->olinkObjectName();
    emitLog(LogLevel::Info, "ClientRegistry.removeObjectSink: " + name);
    removeEntry(name);
}

IObjectSink* ClientRegistry::getObjectSink(std::string name)
{
    emitLog(LogLevel::Info, "ClientRegistry.getObjectSink: " + name);
    return entry(name).sink;
}

ClientNode* ClientRegistry::getClientNode(std::string name)
{
    emitLog(LogLevel::Info, "ClientRegistry.getClientNode: " + name);
    return entry(name).node;
}

ClientNode* ClientRegistry::getClientNode(IObjectSink* sink) {
    return getClientNode(sink->olinkObjectName());
}

SinkToClientEntry& ClientRegistry::entry(std::string name) {
    std::string resource = Name::resourceFromName(name);
    if (m_entries.count(resource) == 0) {
        m_entries[resource] = SinkToClientEntry();
    }
    return m_entries[resource];
}

void ClientRegistry::removeEntry(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if (m_entries.count(resource) > 0) {
        m_entries.erase(resource);
    }
}

}} //namespace ApiGear::ObjectLink