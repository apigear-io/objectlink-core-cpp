#include "sourceregistry.h"
#include <iostream>
namespace ApiGear { namespace ObjectLink {


ObjectSourceRegistry::ObjectSourceRegistry(std::string name)
    : BaseRegistry(name)
{
    SourceRegistryManager::get().setRegistry(name, this);
}

ObjectSourceRegistry::~ObjectSourceRegistry()
{
    SourceRegistryManager::get().unsetRegistry(name());
}

void ObjectSourceRegistry::addSource(std::string name, IObjectSource *source) {
    emitLog(LogLevel::Info, "addObjectSource: " + name);
    std::string resource = Name::resourceFromName(name);
    SourceToNodesEntry entry;
    entry.source = source;
    m_entries[resource] = entry;
}

void ObjectSourceRegistry::removeSource(std::string name) {
    std::string resource = Name::resourceFromName(name);
    m_entries.erase(resource);
}


IObjectSource *ObjectSourceRegistry::getSource(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_entries.count(resource) != 0) {
        return m_entries[resource].source;
    }
    return nullptr;
}

void ObjectSourceRegistry::attachNode(std::string name, ObjectSourceNode *node)
{
    std::cout << "link source" << name;
    std::string resource = Name::resourceFromName(name);
    if(m_entries.count(resource) != 0) {
        return m_entries[resource].nodes.push_back(node);
    } else {
        std::cout << "no source to link, name" << name;
    }
}

void ObjectSourceRegistry::detachNode(std::string name, ObjectSourceNode *node)
{
    std::string resource = Name::resourceFromName(name);
    if(m_entries.count(resource) != 0) {
        m_entries[resource].nodes.remove(node);
    }
}

std::list<ObjectSourceNode *> ObjectSourceRegistry::getNodes(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_entries.count(resource) != 0) {
        return m_entries[resource].nodes;
    }
    return {};
}

ObjectSourceRegistry *ObjectSourceRegistry::registry(std::string name)
{
    return SourceRegistryManager::get().getRegistry(name);
}

void SourceRegistryManager::setRegistry(std::string name, ObjectSourceRegistry *registry)
{
    if(m_registries.count(name) == 0) {
        m_registries[name] = registry;
    } else {
        std::cout << "error: " << "registry exits" << name;
    }
}

void SourceRegistryManager::unsetRegistry(std::string name)
{
    if(m_registries.count(name) == 1) {
        m_registries.erase(name);
    } else {
        std::cout << "error: " << "can not unset registry which does not exits" << name;

    }
}

SourceRegistryManager &SourceRegistryManager::get()
{
    static SourceRegistryManager m;
    return m;
}

ObjectSourceRegistry *SourceRegistryManager::getRegistry(std::string name)
{
    if(m_registries.count(name) == 1) {
        return m_registries[name];
    }
    return nullptr;
}

} } // ApiGear::ObjectLink


