#include "remoteregistry.h"
#include "iremotenode.h"
#include "iobjectsource.h"

namespace ApiGear {
namespace ObjectLink {

void RemoteRegistry::addSource(std::weak_ptr<IObjectSource> source)
{
    auto lockedSource = source.lock();
    if (!lockedSource){
        emitLog(LogLevel::Warning, "Trying to add source to registry, but source already expired");
        return;
    }
    const auto& objectId = lockedSource->olinkObjectName();
    emitLog(LogLevel::Info, "RemoteRegistry.addObjectSource: " + objectId);
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(200)))
    {
        auto found = m_entries.find(objectId);
        auto anyAdded = found != m_entries.end() && !found->second.source.expired();
        auto alreadyAdded = anyAdded && found->second.source.lock() == lockedSource;
        lock.unlock();
        if (alreadyAdded)
        {
            return;
        } else if (anyAdded) {
            emitLog(LogLevel::Info, objectId + " has already a source, source object not changed. Please remove first the existing source. Have in mind, that all associated nodes will also be removed and must be added again." );
            return;
        } 
        SourceToNodesEntry entry;
        entry.source = source;

        if (lock.try_lock_for(std::chrono::milliseconds(200)))
        {
            m_entries[objectId] = entry;
            lock.unlock();
        }
    }
}

void RemoteRegistry::removeSource(const std::string& objectId)
{
    emitLog(LogLevel::Info, "RemoteRegistry.removeObjectSource: " + objectId);
    removeEntry(objectId);
}

std::weak_ptr<IObjectSource> RemoteRegistry::getSource(const std::string& objectId)
{
    emitLog(LogLevel::Info, "RemoteRegistry.getObjectSource: " + objectId);
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(200)))
    {
        auto found = m_entries.find(objectId);
        auto source = found != m_entries.end() ? found->second.source : std::weak_ptr<IObjectSource>();
        lock.unlock();
        return source;
    }
    else {
        emitLog(LogLevel::Info, "RemoteRegistry.getObjectSource, registry is busy: " + objectId);
    }
    return {};
}

std::vector< std::weak_ptr<IRemoteNode>> RemoteRegistry::getNodes(const std::string& objectId)
{
    emitLog(LogLevel::Info, "RemoteRegistry.getRemoteNodes: " + objectId);
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(200)))
    {
        auto found = m_entries.find(objectId);
        if (found != m_entries.end())
        {
            auto nodes = found->second.nodes;
            lock.unlock();
            return nodes;
        } else {
            lock.unlock();
        }
    }
    else {
        emitLog(LogLevel::Info, "RemoteRegistry.getNodes, registry is busy, no nodes searched for : " + objectId);
    }
    return {};
}

std::vector<std::string> RemoteRegistry::getObjectIds(std::weak_ptr<IRemoteNode> node)
{
    std::vector<std::string> ids;
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(200)))
    {
        for (auto& entry : m_entries) {
            auto nodeIsAdded = std::find_if(entry.second.nodes.begin(),
                                            entry.second.nodes.end(),
                                            [node](auto element){ return !node.expired() && node.lock() == element.lock(); })
                               != entry.second.nodes.end();

            if (nodeIsAdded) {
                ids.push_back(entry.first);
            }
        }
        lock.unlock();
    }
    else {
        emitLog(LogLevel::Info, "RemoteRegistry.getObjectsId, registry is busy: no ids for node searched ");
    }
    return ids;
}

void RemoteRegistry::addNodeForSource(std::weak_ptr<IRemoteNode> node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "RemoteRegistry.linkRemoteNode: " + objectId);
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(200)))
    {
        auto found = m_entries.find(objectId);
        if (found != m_entries.end())
        {
            auto alreadyAdded = std::find_if(found->second.nodes.begin(),
                                             found->second.nodes.end(),
                                              [node](auto element){ return !node.expired() && node.lock() == element.lock();}) 
                                != found->second.nodes.end();
            if (!alreadyAdded)
            {
                found->second.nodes.push_back(node);
            }
            lock.unlock();
        }
        else
        {
            lock.unlock();
            emitLog(LogLevel::Info, "RemoteRegistry.linkRemoteNode, node not added, no source for: " + objectId);
        }
    } else {
        emitLog(LogLevel::Info, "RemoteRegistry.linkRemoteNode, add node request denied, registry is busy: " + objectId);
    }
}

void RemoteRegistry::removeNodeFromSource(std::weak_ptr<IRemoteNode> node, const std::string& objectId)
{
    emitLog(LogLevel::Info, "RemoteRegistry.removeNodeFromSource: " + objectId);
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(1000)))
    {
        auto found = m_entries.find(objectId);
        if (found != m_entries.end())
        {
            auto nodeInCollection = std::find_if(found->second.nodes.begin(),
                                    found->second.nodes.end(),
                                    [node](auto element){ return !node.expired() && node.lock() == element.lock(); });
            if (nodeInCollection != found->second.nodes.end()){
                found->second.nodes.erase(nodeInCollection);
            }
        }
        lock.unlock();
    }
    else {
        emitLog(LogLevel::Info, "RemoteRegistry.removeNodeFromSource, registry is busy node not removed! : " + objectId);
    }
}

void RemoteRegistry::removeEntry(const std::string& objectId)
{
    std::unique_lock<std::timed_mutex> lock(m_entriesMutex, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(1000)))
    {
        auto found = m_entries.find(objectId);
        if (found != m_entries.end()) {
            m_entries.erase(found);
        }
        lock.unlock();
    }
    else {
        emitLog(LogLevel::Info, "RemoteRegistry.removeEntry, registry is busy object not removed!: " + objectId);
    }
}

} } // Apigear::ObjectLink