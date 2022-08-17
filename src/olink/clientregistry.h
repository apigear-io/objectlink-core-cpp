#pragma once

#include "core/olink_common.h"
#include "core/basenode.h"
#include <map>


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

    virtual ~ ClientRegistry() = default;

    void attachClientNode(ClientNode* node);

    void detachClientNode(ClientNode* node);

    void linkClientNode(std::string name, ClientNode* node);

    void unlinkClientNode(std::string name, ClientNode* node);

    ClientNode* addObjectSink(IObjectSink* sink);

    void removeObjectSink(IObjectSink* sink);

    IObjectSink* getObjectSink(std::string name);

    ClientNode* getClientNode(std::string name);

    ClientNode* getClientNode(IObjectSink* sink);

    SinkToClientEntry& entry(std::string name);

    void removeEntry(std::string name);

private:
    std::map <std::string, SinkToClientEntry> m_entries;
};

} } // ApiGear::ObjectLink