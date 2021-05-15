#pragma once

#include "sourcetypes.h"


namespace ApiGear { namespace ObjectLink {

class Service;

// registry for remote objects
class SourceRegistry {
private:
public:
    SourceRegistry(std::string name);
    virtual ~SourceRegistry();
    void addObjectSource(std::string name, IObjectLinkSource* listener);
    void removeObjectSource(std::string name);
    IObjectLinkSource* objectSource(std::string name);
    std::string name() const;

    void linkSource(std::string name, IService* service);
    void unlinkSource(std::string name, IService *service);
    std::list<IService*> objectServices(std::string name);
private:
    std::map<std::string, SourceToServiceLink> m_sources;
    std::string m_name;
};

class SourceRegistryManager {
public:
    void setRegistry(std::string name, SourceRegistry* registry);
    void unsetRegistry(std::string name);
    static SourceRegistryManager& get();
    SourceRegistry* registry(std::string name="");
private:
    std::map<std::string, SourceRegistry*> m_registries;
};

} } // ApiGear::ObjectLink

