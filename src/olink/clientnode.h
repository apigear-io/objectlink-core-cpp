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
 /**
 * Registry which manages a client to sink associations.
 * Registry is global and only one sink with unique names can be registered.
 * A sink has always none or one client.
 */
class OLINK_EXPORT ClientNode : public BaseNode, public IClientNode
{
public:
    ClientNode(ClientRegistry& registry);
    virtual ~ClientNode() override;

    /**
     * Use this method to request linking this ClientNode with a Sink Object for given name.
     * @param name. An identifier, unique in ClientRegistry, that is used to connect a ClientNode with Sink Object.
     * This node will be registered for name identifier. Also it is expected that the sink is or will be registered
     * in registry for this name.
     */
    void linkNode(const std::string& interfaceId);
    /**
     * Use this function to inform the ClientNode that the connection is established.
     * For all Sinks associated with this client there will be sent request to link with the service on server side.
     */
    void connectionEstablished();
    /**
     * Use this function to inform all the services on the server side that the client is no longer using the connection and
     * inform the associated sinks that the link was released. 
     */
    void connectionToBeReleased();

    /** IClientNode::linkRemote implementation. */
    void linkRemote(const std::string& interfaceId) override;
    /** IClientNode::unlinkRemote implementation. */
    void unlinkRemote(const std::string& interfaceId) override;
    /** IClientNode::invokeRemote implementation. */
    void invokeRemote(const std::string& methodId, nlohmann::json args=nlohmann::json{}, InvokeReplyFunc func=nullptr) override;
    /** IClientNode::setRemoteProperty implementation. */
    void setRemoteProperty(const std::string& propertyId, nlohmann::json value) override;

     /* The registry in which client is registerd*/
    ClientRegistry& registry();
protected:
    /** IProtocolListener::handleInit implementation */
    void handleInit(const std::string& interfaceId, const nlohmann::json& props) override;
    /** IProtocolListener::handlePropertyChange implementation */
    void handlePropertyChange(const std::string& propertyId, const nlohmann::json& value) override;
    /** IProtocolListener::handleInvokeReply implementation */
    void handleInvokeReply(int requestId, const std::string& methodId, const nlohmann::json& value) override;
    /** IProtocolListener::handleSignal implementation */
    void handleSignal(const std::string& signalId, const nlohmann::json& args) override;
    /** IProtocolListener::handleError implementation */
    void handleError(int msgType, int requestId, const std::string& error) override;

    /**
     * Returns a request id for outgoing messages.
     * @return an unique, non negative id.
     */
    int nextRequestId();
private:
    /* The registry in which client is registerd*/
    ClientRegistry& m_registry;

    /* Value of lsat request id.*/
    int m_nextRequestId;
    /** Collection of callbacks for method replies that client is waiting for associated with the id for invokation request message.*/
    std::map<int,InvokeReplyFunc> m_invokesPending;
};



} } // ApiGear::ObjectLink
