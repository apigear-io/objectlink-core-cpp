#pragma once

#include "sourcetypes.h"


namespace ApiGear { namespace ObjectLink {

class ObjectLinkSession;

// registry for remote objects
class ObjectLinkSourceRegistry: public IObjectLinkService {
public:
    void addObjectSource(std::string name, IObjectLinkSource* listener);
    void removeObjectSource(std::string name);
    IObjectLinkSource* objectSource(std::string name);

    void linkSource(std::string name, ObjectLinkSession* service);
    void unlinkSource(std::string name, ObjectLinkSession *service);
    std::list<ObjectLinkSession*> objectServices(std::string name);
    // IServiceObjectNotifier interface
public:
    void notifyPropertyChange(std::string name, json value) override;
    void notifySignal(std::string name, json args) override;
private:
    std::map<std::string, SourceToSessionLink> m_sources;
};

} } // ApiGear::ObjectLink

