#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {


// member of service object
// distribute signals/propetty changes
class IServiceIO {
public:
    virtual ~IServiceIO();
    virtual void notifyPropertyChange(std::string name, json value) = 0;
    virtual void notifySignal(std::string name, json args) = 0;
    virtual void emitWrite(json j) = 0;

};

// impemented by remote object
// called from object service
class ISource {
public:
    virtual ~ISource();
    virtual std::string getObjectName() = 0;
    virtual json invoke(std::string name, json args) = 0;
    virtual void setProperty(std::string name, json value) = 0;
    virtual void linked(std::string name, IServiceIO* notifier) = 0;
    virtual void unlinked(std::string name) = 0;
    virtual json collectProperties() = 0;
};

// links an object to services
class SourceToServiceLink {
public:
    virtual ~SourceToServiceLink();
    ISource* source;
    std::list<IServiceIO*> services;
};


} } // ApiGear::ObjectLink
