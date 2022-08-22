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
     * @param interfaceId. An identifier of an interface, used to find service on server side with a matching interfaceId.
     *   Contains the module name and the interface name.
     */
    virtual void linkRemote(const std::string& interfaceId) = 0;
    /**
    * Sends a message to inform that client no longer uses the connection to server side.
    * @param interfaceId. An identifier of an interface, used to find service on server side with a matching interfaceId.
    *   Contains the module name and the interface name.
    */
    virtual void unlinkRemote(const std::string& interfaceId) = 0;
    /**
     * Requests a service to invoke a method on a server side.
     * @param methodId Identifier that consists of the interface identifier and the name of the method for which the ivoke request is sent.
     * @param args The arguments with which method should be invoked on server side.
     * @param func a handler for a invokeReplyMessage.
     */
    virtual void invokeRemote(const std::string& methodId, nlohmann::json args = nlohmann::json{}, InvokeReplyFunc func = nullptr) = 0;
    /**
     * Request a service on server side to change a property to requested value.
     * Once the request is accepted and property is changed the server side will send propertyChangeMessage.
     * @param propertyId Identifier that consists of the interface identifier and the name of the property for which change request is sent.
     * @param value The value of property to set to.
     */
    virtual void setRemoteProperty(const std::string& propertyId, nlohmann::json value) = 0;
};



}} // ApiGear::ObjectLink