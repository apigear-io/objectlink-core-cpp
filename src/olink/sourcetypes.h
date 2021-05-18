#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {

class SourceLink;


// passed into source object
// distribute signals/propetty changes
class ISourceLink {
public:
    virtual ~ISourceLink();
    virtual void notifyPropertyChange(std::string name, json value) = 0;
    virtual void notifySignal(std::string name, json args) = 0;
};

// impemented by source object
// called from object link
class IObjectSource {
public:
    virtual ~IObjectSource();
    virtual std::string getObjectName() = 0;
    virtual json invoke(std::string name, json args) = 0;
    virtual void setProperty(std::string name, json value) = 0;
    virtual void linked(std::string name, ISourceLink* link) = 0;
    virtual void unlinked(std::string name) = 0;
    virtual json collectProperties() = 0;
};

// links an source object to ObjectLinks
class SourceToLinksEntry {
public:
    virtual ~SourceToLinksEntry();
    IObjectSource* source;
    std::list<SourceLink*> links;
};


} } // ApiGear::ObjectLink
