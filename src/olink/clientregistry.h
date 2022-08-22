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
 * @brief internal structure to manage sink/node associations
 * one onject sink can only be linked to one node
 */
struct OLINK_EXPORT SinkToClientEntry{
    SinkToClientEntry()
        : sink(nullptr)
        , node(nullptr)
        {}
    IObjectSink *sink;
    ClientNode *node;
};

/**
 * @brief client side sink registry
 */
class OLINK_EXPORT ClientRegistry: public Base {
public:

    ClientRegistry();

    virtual ~ClientRegistry() = default;

    // TODO remove the function
    void attachClientNode(ClientNode& node);
    // TODO rename unlinkClientNodeForAllSinks, or remove and instead use ~ foreach(getObjects()) {unlinkClientNode(object, node}
    void detachClientNode(ClientNode& node);

    void linkClientNode(const std::string& interfaceId, ClientNode& node);

    void unlinkClientNode(const std::string& interfaceId, ClientNode& node);
    // TODO rename: createRecord(sink)
    void addObjectSink(IObjectSink& sink);
    // TODO rename: removeRecord(interfaceId)
    void removeObjectSink(IObjectSink& sink);

    IObjectSink* getObjectSink(const std::string& interfaceId);

    std::vector<std::string> getObjects(ClientNode& node);

    ClientNode* getClientNode(const std::string& interfaceId);

private:
    void removeEntry(const std::string& interfaceId);
    SinkToClientEntry& entry(const std::string& interfaceId);

    std::map <std::string, SinkToClientEntry> m_entries;
};

} } // ApiGear::ObjectLink