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
 * Client node separates the object sink from a network related implementation, it provides functionality 
 * for sending and reciving messages. It is associated with one socket to handle incoming messages
 * and to write messages requested by sinks that are using this client node.
 * The network implementation should deliver a write function for the node  to allow sendign messages
 * see BaseNode::emitWrite and BaseNode::onWrite.
 * A sink to receive a handler call is chosen based on registry entries, and objectId retrived from incoming message.
 * To use objectSink with this client, client needs to be registered in client registry for an object
 * see ClientRegistry::ilnkToObject function.
 */
class OLINK_EXPORT ClientNode : public BaseNode, public IClientNode
{
public:
    ClientNode(ClientRegistry& registry);
    /** dtor 
    * informs the user on server side that connection is no longer used,
    * also inform all the sinks that connection was released 
    * and unlinks from registry for every object that used it.
    */
    virtual ~ClientNode() override;

    /**
     * Use this function to inform the ClientNode that the connection is established.
     * For all sinks associated with this client there will be sent request to link with the service on server side.
     */
    void connectionEstablished();
    /**
     * Use this function to inform all the services on the server side that the client is no longer using the connection
     * and unlink from registry for every object that was using this node.
     * The sink is not notified that the connection is closed.
     */
    void connectionToBeReleased();

    /** IClientNode::linkRemote implementation. */
    void linkRemote(const std::string& objectId) override;
    /** IClientNode::unlinkRemote implementation. */
    void unlinkRemote(const std::string& objectId) override;
    /** IClientNode::invokeRemote implementation. */
    void invokeRemote(const std::string& methodId, nlohmann::json args=nlohmann::json{}, InvokeReplyFunc func=nullptr) override;
    /** IClientNode::setRemoteProperty implementation. */
    void setRemoteProperty(const std::string& propertyId, nlohmann::json value) override;
    /** IClientNode::hasNetworkConnection implementation. */
    bool hasNetworkConnection() override;

     /* The registry in which client is registerd*/
    ClientRegistry& registry();

protected:
    /** IProtocolListener::handleInit implementation */
    void handleInit(const std::string& objectId, const nlohmann::json& props) override;
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
     * @return a unique, non negative id.
     */
    int nextRequestId();
private:
    /* The registry in which client is registerd and which provides sinks connected with this node*/
    ClientRegistry& m_registry;
    /** Stores information if connections is running*/
    bool m_hasNetworkConnection = false;

    /* Value of lsat request id.*/
    int m_nextRequestId;
    /** Collection of callbacks for method replies that client is waiting for associated with the id for invokation request message.*/
    std::map<int,InvokeReplyFunc> m_invokesPending;
};



} } // ApiGear::ObjectLink
