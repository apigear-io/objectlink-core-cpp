#include "sourceregistry.h"
#include "session.h"
#include "core/messages.h"

namespace ApiGear { namespace ObjectLink {


void ObjectLinkSourceRegistry::addObjectSource(std::string name, IObjectLinkSource *listener) {
    std::string resource = Message::resourceFromName(name);
    SourceToSessionLink link;
    link.source = listener;
    m_sources[resource] = link;
}

void ObjectLinkSourceRegistry::removeObjectSource(std::string name) {
    std::string resource = Message::resourceFromName(name);
    m_sources.erase(resource);
}


IObjectLinkSource *ObjectLinkSourceRegistry::objectSource(std::string name)
{
    std::string resource = Message::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].source;
    }
    return nullptr;
}

void ObjectLinkSourceRegistry::linkSource(std::string name, ObjectLinkSession *service)
{
    std::string resource = Message::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions.push_back(service);
    }
}

void ObjectLinkSourceRegistry::unlinkSource(std::string name, ObjectLinkSession *service)
{
    std::string resource = Message::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        m_sources[resource].sessions.remove(service);
    }
}

std::list<ObjectLinkSession *> ObjectLinkSourceRegistry::objectServices(std::string name)
{
    std::string resource = Message::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions;
    }
    return {};
}

} } // ApiGear::ObjectLink


