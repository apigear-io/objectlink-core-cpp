#pragma once

#include "nlohmann/json.hpp"
#include "core/olink_common.h"
#include "core/types.h"
#include <string>

namespace ApiGear{
namespace ObjectLink{


/**
 * @brief Describes outgoing part of the protocol for client side.
 * Implementation should send apropiate messages to server side.
 */
class OLINK_EXPORT IClientNode {
public:
    virtual ~IClientNode() = default;
    /**
     * Sends a message to request linking this client with a server side.
     * @param objectId. An identifier of an object, used to find service object on server side with a matching objectId.
     *   Typically contains the module name and the object name.
     */
    virtual void linkRemote(const std::string& objectId) = 0;
    /**
    * Sends a message to inform that client no longer uses the connection to server side.
    * @param objectId. An identifier of an object, used to find service object on server side with a matching objectId.
    *   Typically contains the module name and the object name.
    */
    virtual void unlinkRemote(const std::string& objectId) = 0;
    /**
     * Requests a service to invoke a method on a server side.
     * @param methodId Identifier that consists of the object identifier and the name of the method for which the invoke request is sent.
     * @param args The arguments with which method should be invoked on server side.
     * @param func a handler for a invokeReplyMessage.
     */
    virtual void invokeRemote(const std::string& methodId, nlohmann::json args = nlohmann::json{}, InvokeReplyFunc func = nullptr) = 0;
    /**
     * Request a service on server side to change a property to requested value.
     * Once the request is accepted and property is changed the server side will send propertyChangeMessage.
     * @param propertyId Identifier that consists of the objectId and the name of the property for which change request is sent.
     * @param value The value of property to set to.
     * 
     * see ApiGear::ObjectLink::Name::createMemberId to create propertyId 
     * see also: ApiGear::ObjectLink::Name::getObjectId, ApiGear::ObjectLink::getMemberName
     */
    virtual void setRemoteProperty(const std::string& propertyId, nlohmann::json value) = 0;

    /** Use this function to check whether the Client Node is ready to send messages through the network */
    virtual bool hasNetworkConnection() = 0 ;
};



}} // ApiGear::ObjectLink