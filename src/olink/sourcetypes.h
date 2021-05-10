#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {


class ObjectLinkSession;

// member of service object
// distribute signals/propetty changes
class IObjectLinkService {
public:
    virtual ~IObjectLinkService();
    virtual void notifyPropertyChange(std::string name, json value) = 0;
    virtual void notifySignal(std::string name, json args) = 0;

};

// impemented by remote object
// called from object service
class IObjectLinkSource {
public:
    virtual ~IObjectLinkSource();
    virtual std::string getObjectName() = 0;
    virtual json invoke(std::string name, json args) = 0;
    virtual void setProperty(std::string name, json value) = 0;
    virtual void linked(std::string name, IObjectLinkService* notifier) = 0;
    virtual void unlinked(std::string name) = 0;
    virtual json collectProperties() = 0;
};

// links an object to services
class SourceToSessionLink {
public:
    virtual ~SourceToSessionLink();
    IObjectLinkSource* source;
    std::list<ObjectLinkSession*> sessions;
};


} } // ApiGear::ObjectLink
