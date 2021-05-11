#include "sourceregistry.h"
#include "session.h"
#include "spdlog/spdlog.h"

namespace ApiGear { namespace ObjectLink {


void ObjectLinkSourceRegistry::addObjectSource(std::string name, IObjectLinkSource *source) {
    std::string resource = Name::resourceFromName(name);
    SourceToSessionLink link;
    link.source = source;
    m_sources[resource] = link;
}

void ObjectLinkSourceRegistry::removeObjectSource(std::string name) {
    std::string resource = Name::resourceFromName(name);
    m_sources.erase(resource);
}


IObjectLinkSource *ObjectLinkSourceRegistry::objectSource(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].source;
    }
    return nullptr;
}

void ObjectLinkSourceRegistry::linkSource(std::string name, ObjectLinkSession *session)
{
    spdlog::info("link source: {}", name);
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions.push_back(session);
    } else {
        spdlog::warn("no source to link, name: {}", name);
    }
}

void ObjectLinkSourceRegistry::unlinkSource(std::string name, ObjectLinkSession *session)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        m_sources[resource].sessions.remove(session);
    }
}

std::list<ObjectLinkSession *> ObjectLinkSourceRegistry::objectServices(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sources.count(resource) != 0) {
        return m_sources[resource].sessions;
    }
    return {};
}

} } // ApiGear::ObjectLink


