#include "sourcenode.h"
#include "sourcelink.h"
#include <iostream>
namespace ApiGear { namespace ObjectLink {


SourceNode::SourceNode(std::string name)
    : ObjectNode(name)
{
    SourceNodeManager::get().setSourceNode(name, this);
}

SourceNode::~SourceNode()
{
    SourceNodeManager::get().unsetSourceNode(nodeName());
}

void SourceNode::addObjectSource(std::string name, IObjectSource *source) {
    emitLog(LogLevel::Info, "addObjectSource: " + name);
    std::string resource = Name::resourceFromName(name);
    SourceToLinksEntry entry;
    entry.source = source;
    m_sourceEntries[resource] = entry;
}

void SourceNode::removeObjectSource(std::string name) {
    std::string resource = Name::resourceFromName(name);
    m_sourceEntries.erase(resource);
}


IObjectSource *SourceNode::getObjectSource(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sourceEntries.count(resource) != 0) {
        return m_sourceEntries[resource].source;
    }
    return nullptr;
}

void SourceNode::attachSourceLink(std::string name, SourceLink *link)
{
    std::cout << "link source" << name;
    std::string resource = Name::resourceFromName(name);
    if(m_sourceEntries.count(resource) != 0) {
        return m_sourceEntries[resource].links.push_back(link);
    } else {
        std::cout << "no source to link, name" << name;
    }
}

void SourceNode::detachSourceLink(std::string name, SourceLink *link)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sourceEntries.count(resource) != 0) {
        m_sourceEntries[resource].links.remove(link);
    }
}

std::list<SourceLink *> SourceNode::getSourceLinks(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sourceEntries.count(resource) != 0) {
        return m_sourceEntries[resource].links;
    }
    return {};
}

SourceNode *SourceNode::getSourceNode(std::string name)
{
    return SourceNodeManager::get().getSourceNode(name);
}

void SourceNodeManager::setSourceNode(std::string name, SourceNode *node)
{
    if(m_nodes.count(name) == 0) {
        m_nodes[name] = node;
    } else {
        std::cout << "error: " << "registry exits" << name;
    }
}

void SourceNodeManager::unsetSourceNode(std::string name)
{
    if(m_nodes.count(name) == 1) {
        m_nodes.erase(name);
    } else {
        std::cout << "error: " << "can not unset registry which does not exits" << name;

    }
}

SourceNodeManager &SourceNodeManager::get()
{
    static SourceNodeManager m;
    return m;
}

SourceNode *SourceNodeManager::getSourceNode(std::string name)
{
    if(m_nodes.count(name) == 1) {
        return m_nodes[name];
    }
    return nullptr;
}

} } // ApiGear::ObjectLink


