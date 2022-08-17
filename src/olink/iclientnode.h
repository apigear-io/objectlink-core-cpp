#pragma once

#include "nlohmann/json.hpp"
#include "core/olink_common.h"
#include "core/types.h"
#include <string>

namespace ApiGear{
namespace ObjectLink{


/**
 * @brief Interface exposed to object sinks to call client functions
 */
class OLINK_EXPORT IClientNode {
public:
    virtual ~IClientNode() = default;

    virtual void linkRemote(std::string name) = 0;

    virtual void unlinkRemote(std::string name) = 0;

    virtual void invokeRemote(std::string name, nlohmann::json args = nlohmann::json{}, InvokeReplyFunc func = nullptr) = 0;

    virtual void setRemoteProperty(std::string name, nlohmann::json value) = 0;
};


}} // ApiGear::ObjectLink