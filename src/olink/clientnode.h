#pragma once

#include "olink/core/types.h"
#include "olink/core/node.h"


namespace ApiGear { namespace ObjectLink {

class ClientNode;
class ClientRegistry;

/**
 * @brief Interface exposed to object sinks to call client functions
 */
class IClientNode {
public:
    virtual ~IClientNode();
    virtual void linkRemote(std::string name) = 0;
    virtual void unlinkRemote(std::string name) = 0;
    virtual void invokeRemote(std::string name, json args=json{}, InvokeReplyFunc func=nullptr) = 0;
    virtual void setRemoteProperty(std::string name, json value) = 0;
};

/**
 * @brief Interface to be implemented by object sinks
 */
class IObjectSink
{
public:
    virtual ~IObjectSink();
    virtual std::string olinkObjectName() = 0;
    virtual void olinkOnSignal(std::string name, json args) = 0;
    virtual void olinkOnPropertyChanged(std::string name, json value) = 0;
    virtual void olinkOnInit(std::string name, json props, IClientNode* node) = 0;
    virtual void olinkOnRelease() = 0;
};

/**
 * @brief internal structure to manage sink/node associations
 * one onject sink can only be linked to one node
 */
struct SinkToClientEntry {
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
class ClientRegistry : public Base {
private:
    ClientRegistry();
public:
    virtual ~ClientRegistry() override;
    static ClientRegistry& get();
    void attachClientNode(ClientNode *node);
    void detachClientNode(ClientNode *node);
    void linkClientNode(std::string name, ClientNode *node);
    void unlinkClientNode(std::string name, ClientNode *node);
    void addObjectSink(IObjectSink *sink);
    void removeObjectSink(IObjectSink *sink);
    IObjectSink *getObjectSink(std::string name);
    void initEntry(std::string name);
    bool hasEntry(std::string name);
    SinkToClientEntry &entry(std::string name);
private:
    std::map<std::string, SinkToClientEntry> m_entries;
};

/**
 * @brief client side node to handle sinks and olink messages
 * A client node is associated with one socket to handle messages and to wrire messages.
 * The client node calls the correct sinks based on registry entries.
 */
class ClientNode : public BaseNode, public IClientNode
{
public:
    ClientNode();
    virtual ~ClientNode() override;
public: // IClientNode
    void linkRemote(std::string name) override;
    void unlinkRemote(std::string name) override;
    void invokeRemote(std::string name, json args=json{}, InvokeReplyFunc func=nullptr) override;
    void setRemoteProperty(std::string name, json value) override;
    ClientRegistry& clientRegistry();
public: // sink registry
    void addObjectSink(IObjectSink *sink);
    void removeObjectSink(IObjectSink *sink);
    IObjectSink* getObjectSink(std::string name);
protected: // IMessageListener
    void handleInit(std::string name, json props) override;
    void handlePropertyChange(std::string name, json value) override;
    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handleError(int msgType, int requestId, std::string error) override;
protected:
    int nextRequestId();
private:
    int m_nextRequestId;
    std::map<int,InvokeReplyFunc> m_invokesPending;
};



} } // ApiGear::ObjectLink
