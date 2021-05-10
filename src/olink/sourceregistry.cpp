#include "sourceregistry.h"
#include "session.h"

namespace ApiGear { namespace ObjectLink {


void ObjectLinkSourceRegistry::addObjectSource(std::string name, IObjectLinkSource *listener) {
    std::string resource = name.substr(0, name.find("/"));
    SourceToSessionLink link;
    link.source = listener;
    m_sources[resource] = link;
}

void ObjectLinkSourceRegistry::removeObjectSource(std::string name) {
    std::string resource = name.substr(0, name.find("/"));
    m_sources.erase(resource);
}

void ObjectLinkSourceRegistry::notifyPropertyChange(std::string name, json value)
{
    std::list<ObjectLinkSession *> services = objectServices(name);
    for(const ObjectLinkSession* service: services) {
    }
}

void ObjectLinkSourceRegistry::notifySignal(std::string name, json args)
{
}


IObjectLinkSource *ObjectLinkSourceRegistry::objectSource(std::string name)
{
    std::string resource = name.substr(0, name.find("/"));
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].source;
    }
    return nullptr;
}

void ObjectLinkSourceRegistry::linkSource(std::string name, ObjectLinkSession *service)
{
    std::string resource = name.substr(0, name.find("/"));
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions.push_back(service);
    }
}

void ObjectLinkSourceRegistry::unlinkSource(std::string name, ObjectLinkSession *service)
{
    std::string resource = name.substr(0, name.find("/"));
    if(m_sources.count(resource) != 0) {
        m_sources[resource].sessions.remove(service);
    }
}


std::list<ObjectLinkSession *> ObjectLinkSourceRegistry::objectServices(std::string name)
{
    std::string resource = name.substr(0, name.find("/"));
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions;
    }
    return {};
}

} } // ApiGear::ObjectLink


