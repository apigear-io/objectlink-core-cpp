#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {

class ObjectSourceNode;


// passed into source object
// distribute signals/propetty changes
class IObjectSourceNode {
public:
    virtual ~IObjectSourceNode();
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
    virtual void linked(std::string name, IObjectSourceNode* node) = 0;
    virtual void unlinked(std::string name) = 0;
    virtual json collectProperties() = 0;
};

// links an source object to ObjectLinks
class SourceToNodesEntry {
public:
    virtual ~SourceToNodesEntry();
    IObjectSource* source;
    std::list<ObjectSourceNode*> nodes;
};


} } // ApiGear::ObjectLink
