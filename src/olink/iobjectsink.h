#pragma once

#include "nlohmann/json.hpp"
#include "core/olink_common.h"
#include <string>

namespace ApiGear{
namespace ObjectLink {

class IClientNode;

/**
* @brief Interface to be implemented by object sinks
*/
class OLINK_EXPORT IObjectSink {
public:
    virtual ~IObjectSink() = default;

    virtual std::string olinkObjectName() = 0;

    virtual void olinkOnSignal(std::string name, nlohmann::json args) = 0;

    virtual void olinkOnPropertyChanged(std::string name, nlohmann::json value) = 0;

    virtual void olinkOnInit(std::string name, nlohmann::json props, IClientNode *node) = 0;

    virtual void olinkOnRelease() = 0;
};

}} // ApiGear::ObjectLink