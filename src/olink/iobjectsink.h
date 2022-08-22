#pragma once

#include "nlohmann/json.hpp"
#include "core/olink_common.h"
#include <string>

namespace ApiGear{
namespace ObjectLink {

class IClientNode;

/**
 * @brief Describes incoming part of the protocol for client side.
 * Implementation should send apropiate messages to server side.
 */
class OLINK_EXPORT IObjectSink {
public:
    virtual ~IObjectSink() = default;

    /**
    * Returns olink object identifier.
    * @return The olink object which is an identifier for service. It is used to connect the client side to the server side service,
    * therefore it has to be the same on both sides and has to allow unambigious linking clients to server side.
    */
    virtual std::string olinkObjectName() = 0;

    /**
    * Handler for serving the singal emission by the service.
    * @param name. The 
    */
    virtual void olinkOnSignal(std::string signalId, nlohmann::json args) = 0;

    virtual void olinkOnPropertyChanged(std::string propertyId, nlohmann::json value) = 0;

    virtual void olinkOnInit(std::string interfaceId, nlohmann::json props, IClientNode *node) = 0;

    virtual void olinkOnRelease() = 0;
};

}} // ApiGear::ObjectLink