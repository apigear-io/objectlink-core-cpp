#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {


// member of service object
// distribute signals/propetty changes
class IService {
public:
    virtual ~IService();
    virtual void notifyPropertyChange(std::string name, json value) = 0;
    virtual void notifySignal(std::string name, json args) = 0;
    virtual void emitWrite(json j) = 0;

};

// impemented by remote object
// called from object service
class IObjectLinkSource {
public:
    virtual ~IObjectLinkSource();
    virtual std::string getObjectName() = 0;
    virtual json invoke(std::string name, json args) = 0;
    virtual void setProperty(std::string name, json value) = 0;
    virtual void linked(std::string name, IService* notifier) = 0;
    virtual void unlinked(std::string name) = 0;
    virtual json collectProperties() = 0;
};

// links an object to services
class SourceToServiceLink {
public:
    virtual ~SourceToServiceLink();
    IObjectLinkSource* source;
    std::list<IService*> services;
};


} } // ApiGear::ObjectLink
