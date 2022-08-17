#pragma once

#include "core/olink_common.h"
#include "iclientnode.h"
#include "core/basenode.h"
#include <map>


namespace ApiGear { namespace ObjectLink {

class ClientNode;
class ClientRegistry;
class IObjectSink;

/**
 * @brief client side node to handle sinks and olink messages
 * A client node is associated with one socket to handle messages and to wrire messages.
 * The client node calls the correct sinks based on registry entries.
 */
class OLINK_EXPORT ClientNode : public BaseNode, public IClientNode
{
public:
    ClientNode(ClientRegistry& registry);
    virtual ~ClientNode() override;
    /**
     * link client node to object source by name
     */
    void linkNode(std::string name);
    /**
     * unlink client node from object source by name
     */
    void unlinkNode(std::string name);
public: // IClientNode
    /**
     * link object source to remote node by name
     * sends Link message
     */
    void linkRemote(std::string name) override;
    /**
     * unlinks object source from remote node by name
     * sends UNKINK message
     */
    void unlinkRemote(std::string name) override;
    /**
     * invokes a remote function by name using arguments.
     * Result is delivered using reply function.
     * sends Invoke message and registers a reply handler (InvokeReply)
     */
    void invokeRemote(std::string name, nlohmann::json args=nlohmann::json{}, InvokeReplyFunc func=nullptr) override;
    /**
     * set remote property using name to value.
     * sends SetProperty message.
     * Changes will be distributed using PropertyChange message.
     */
    void setRemoteProperty(std::string name, nlohmann::json value) override;
    /**
     * Registry which manages a client to sink associations.
     * Registry is global and only one sink with unique names can be registered.
     * A sink has always none or one client.
     */
    ClientRegistry& registry();
public: // sink registry
    /**
     * Adds object sink to global registry.
     * Return a client node, when previously registered using linkNode
     */
    ClientNode* addObjectSink(IObjectSink* sink);
    /**
     * Removed object sink from global registry.
     */
    void removeObjectSink(IObjectSink *sink);
    /**
     * Gets object sink using unique sink name
     */
    IObjectSink* getObjectSink(std::string name);
protected: // IMessageListener
    /**
     * handles remote init message.
     * Calls the object sink init function.
     */
    void handleInit(std::string name, nlohmann::json props) override;
    /**
     * handles remote property change message
     * Calls the object sink property change function
     */
    void handlePropertyChange(std::string name, nlohmann::json value) override;
    /**
     * handles remote invoke reply message
     * Lookups the reply func and calls the function to deliver the value
     */
    void handleInvokeReply(int requestId, std::string name, nlohmann::json value) override;
    /**
     * handles remote signal message
     * Calls the object sink signal function
     */
    void handleSignal(std::string name, nlohmann::json args) override;
    /**
     * handles remote error message
     * Stores the error function. If request id, removes also the reply handler.
     */
    void handleError(int msgType, int requestId, std::string error) override;
protected:
    /**
     * returns new request id.
     * Request id is always greater 0.
     */
    int nextRequestId();
private:
    int m_nextRequestId;
    std::map<int,InvokeReplyFunc> m_invokesPending;
    ClientRegistry* m_registry;
};



} } // ApiGear::ObjectLink
