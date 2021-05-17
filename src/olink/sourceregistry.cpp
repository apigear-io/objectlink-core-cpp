#include "sourceregistry.h"
#include "service.h"
#include <iostream>
namespace ApiGear { namespace ObjectLink {


SourceRegistry::SourceRegistry(std::string name)
    : m_name(name)
    , m_logFunc(nullptr)
{
    SourceRegistryManager::get().setRegistry(name, this);
}

SourceRegistry::~SourceRegistry()
{
    SourceRegistryManager::get().unsetRegistry(name());
}

void SourceRegistry::onLog(LogWriterFunc func)
{
    m_logFunc = func;
}

void SourceRegistry::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }

}

void SourceRegistry::addObjectSource(std::string name, ISource *source) {
    emitLog(LogLevel::Info, "addObjectSource: " + name);
    std::string resource = Name::resourceFromName(name);
    SourceToServiceLink link;
    link.source = source;
    m_sources[resource] = link;
}

void SourceRegistry::removeObjectSource(std::string name) {
    std::string resource = Name::resourceFromName(name);
    m_sources.erase(resource);
}


ISource *SourceRegistry::objectSource(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].source;
    }
    return nullptr;
}

std::string SourceRegistry::name() const
{
    return m_name;
}

void SourceRegistry::linkSource(std::string name, IServiceIO *service)
{
    std::cout << "link source" << name;
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].services.push_back(service);
    } else {
        std::cout << "no source to link, name" << name;
    }
}

void SourceRegistry::unlinkSource(std::string name, IServiceIO *service)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        m_sources[resource].services.remove(service);
    }
}

std::list<IServiceIO *> SourceRegistry::objectServices(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].services;
    }
    return {};
}

void SourceRegistryManager::setRegistry(std::string name, SourceRegistry *registry)
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

SourceRegistry *SourceRegistryManager::registry(std::string name)
{
    if(m_registries.count(name) == 1) {
        return m_registries[name];
    }
    return nullptr;
}

} } // ApiGear::ObjectLink


