#pragma once

#include "sourceregistry.h"
#include "olink/shared/baseregistry.h"
#include "sourcetypes.h"


namespace ApiGear { namespace ObjectLink {

class ObjectSourceNode;

// registry for remote objects
class ObjectSourceRegistry: public BaseRegistry {
private:
public:
    ObjectSourceRegistry(std::string name);
    virtual ~ObjectSourceRegistry() override;

    void addSource(std::string name, IObjectSource* source);
    void removeSource(std::string name);
    IObjectSource* getSource(std::string name);

    void attachNode(std::string name, ObjectSourceNode* node);
    void detachNode(std::string name, ObjectSourceNode *node);
    std::list<ObjectSourceNode*> getNodes(std::string name);

    static ObjectSourceRegistry *registry(std::string name);
private:
    std::map<std::string, SourceToNodesEntry> m_entries;
};

class SourceRegistryManager {
public:
    void setRegistry(std::string name, ObjectSourceRegistry* registry);
    void unsetRegistry(std::string name);
    static SourceRegistryManager& get();
    ObjectSourceRegistry* getRegistry(std::string name);
private:
    std::map<std::string, ObjectSourceRegistry*> m_registries;
};

} } // ApiGear::ObjectLink

