#pragma once

#include "sourcetypes.h"


namespace ApiGear { namespace ObjectLink {

class ServiceIO;

// registry for remote objects
class SourceRegistry {
private:
public:
    SourceRegistry(std::string name);
    virtual ~SourceRegistry();
    void onLog(LogWriterFunc func);
    void emitLog(LogLevel level, std::string msg);
    void addObjectSource(std::string name, ISource* listener);
    void removeObjectSource(std::string name);
    ISource* objectSource(std::string name);
    std::string name() const;

    void linkSource(std::string name, IServiceIO* service);
    void unlinkSource(std::string name, IServiceIO *service);
    std::list<IServiceIO*> objectServices(std::string name);
private:
    std::map<std::string, SourceToServiceLink> m_sources;
    std::string m_name;
    LogWriterFunc m_logFunc;
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

