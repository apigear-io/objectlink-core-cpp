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
 * Each object is registered with its id, available with olinkObjectName() call.
 * This id has to be unique in the registry, only first object with same id will be registered.
 * A client node may be used for many objects.
 * Register your object and a client node separately: an object with addSink function and 
 * the client node with setNode, which requires also the target objectId.
 * The order of registration is not relevant.
 * Sink object should always be removed from registry before deleting it.
 */
class OLINK_EXPORT ClientRegistry: public LoggerBase {
public:
    /**dtor*/
    virtual ~ClientRegistry() = default;

    /**
    * Set ClientNode for a sink object registered with objectId
    * @param objectId An id of object, for which the node should be added.
    *   If node exist for given objectId node is not added.
    */
    void setNode(ClientNode& node, const std::string& objectId);
    /**
    * Unset the ClientNode from registry for objectId.
    * @param objectId An id of object, for which the node should be removed.
    *   If there is no node registered for objectId, or registered node is different, no action is taken.
    *   If entry was found for given objectId and the node is the same object, the node is removed from registry,
    *  but entry stays - the sink is still registered.
    */
    void unsetNode(ClientNode& node, const std::string& objectId);

    /**
    * Registers a Sink Object with its objectId.
    * Sink Object must provide objectId that is unique in this registry.
    * @param objectId An id of object, for which the node should be added.
    *   If object already exist for given objectId this sinkObject is not added.
    */
    void addSink(IObjectSink& sink);

    /**
    * Removes a Sink Object from registry for objectId.
    * @param objectId An id of object, for which the node should be removed.
    *   If there is no node registered for objectId, or registered node is different, no action is taken.
    *   If entry was found for given objectId and the node is the same object, the node is removed,
    *  but entry stays in the registry. The sink is still registered.
    */
    void removeSink(const std::string& objectId);

    /**
    * Returns a sink object for the given objectId.
    * @param objectId Identifier of a sink Object.
    * @return Sink Object with given objectId.
    */
    IObjectSink* getSink(const std::string& objectId);

    /**
    * Returns List of ids of all objects that are using given node.
    * @param node A node for which objects using it should be found.
    * @return a collection of Ids of all the objects that use given node.
    */
    std::vector<std::string> getObjectIds(ClientNode& node);

    /**
    * Returns ClientNode for given objectId.
    * @param objectId An id of object, for which the node should be searched.
    * @return A node found for an objectId or nullptr if there is no objectId in registry.
    */
    ClientNode* getNode(const std::string& objectId);
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
    * They objectId must be unique for whole registry, only one Object sink and one ClientNode
    * can be registered for one objectId
    */
    std::map <std::string, SinkToClientEntry> m_entries;
};

} } // ApiGear::ObjectLink
