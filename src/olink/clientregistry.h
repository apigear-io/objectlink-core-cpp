#pragma once

#include "core/olink_common.h"
#include "core/basenode.h"
#include <map>
#include <vector>


namespace ApiGear {
namespace ObjectLink {

class IObjectSink;
class ClientNode;

/**
 * A Registry is a global storage to keep track of objects stored as objectSink for the messages 
 * and a client node associated with that objectSink.
 * Each object can use only one client node.
 * Each object is registerd with its id, available with olinkObjectName() call.
 * This id has to be unique in the registry, only first object with same id will be registerd.
 * A client node may be used for many objects.
 * Register your object and a client node separately: an object with addObject function and 
 * the client node with setNode, which requires also the target objectId.
 * The order of registration is not relevant.
 */
class OLINK_EXPORT ClientRegistry: public LoggerBase {
public:
    /**dtor*/
    virtual ~ClientRegistry() = default;

    /**
    * Adds ClientNode to registry for objectId.
    * @param objectId An id of object, for which the node should be added.
    *   If node exist for given objectId node is not added.
    */
    void setNode(ClientNode& node, const std::string& objectId);
    /**
    * Removes ClientNode from registry for objectId.
    * @param objectId An id of object, for which the node should be removed.
    *   If there is no node registerd for objectId, or registered node is different, no action is taken.
    *   If entry was found for given objectId and the node is the same object, the node is removed,
    *  but entry stays in the registry. The sink is still registered.
    */
    void unsetNode(ClientNode& node, const std::string& objectId);

    /**
    * Registers a SinkObject with its objectId.
    * SinkObject must provide objectId that is uniqe in this registry.
    * @param objectId An id of object, for which the node should be added.
    *   If object already exist for given objectId this sinkObject is not added.
    */
    void addObject(IObjectSink& sink);

    /**
    * Removes an object from registry for objectId.
    * @param objectId An id of object, for which the node should be removed.
    *   If there is no node registerd for objectId, or reqistered node is different, no action is taken.
    *   If entry was found for given objectId and the node is the same object, the node is removed,
    *  but entry stays in the registry. The sink is still registered.
    */
    void removeObject(const std::string& objectId);

    /**
    * Returns an object sink for the given objectId.
    * @param objectId Identifier of an objectSink.
    * @return ObjectSink with given objectId.
    */
    IObjectSink* getObjectSink(const std::string& objectId);

    /**
    * Returns Ids of all objects that are using given node.
    * @param node A node for which objects using it should be found.
    * @return a collection of Ids of all the objects that use given node.
    */
    std::vector<std::string> getObjectsId(ClientNode& node);

    /**
    * Returns ClientNode for given objectId.
    * @param objectId An id of object, for which the node should be searched.
    * @return A node found for an objectId or nullptr if there is no objectId in registry.
    */
    ClientNode* getClientNode(const std::string& objectId);
private:
    /**
     * Internal structure to manage sink/node associations
     */
    struct OLINK_EXPORT SinkToClientEntry{
        IObjectSink* sink = nullptr;
        ClientNode* node = nullptr;
    };

    /**
    * Removes entry for given objectId.
    * @param objectId. Identifier for which an entry would be removed.
    * If entry not found no action is made.
    */
    void removeEntry(const std::string& objectId);
    /**
    * Returns an entry for given objectId.
    * If no entry found, a new empty SinkToClientEntry is added for objectId
    * @param objectId unique objectId, for which ClientNode or IObjectSink were registered.
    * @return entry for objectId.
    */
    SinkToClientEntry& entry(const std::string& objectId);

    /**
    * Collection of registered ObjectSinks for given objectId with ClientNodes they use.
    * They objectId must be uniqe for whole registry, only one Object sink and one ClientNode
    * can be registered for one objectId
    */
    std::map <std::string, SinkToClientEntry> m_entries;
};

} } // ApiGear::ObjectLink
