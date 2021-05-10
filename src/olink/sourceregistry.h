#pragma once

#include "sourcetypes.h"


namespace ApiGear { namespace ObjectLink {

class ObjectLinkSession;

// registry for remote objects
class ObjectLinkSourceRegistry {
public:
    void addObjectSource(std::string name, IObjectLinkSource* listener);
    void removeObjectSource(std::string name);
    IObjectLinkSource* objectSource(std::string name);

    void linkSource(std::string name, ObjectLinkSession* service);
    void unlinkSource(std::string name, ObjectLinkSession *service);
    std::list<ObjectLinkSession*> objectServices(std::string name);
private:
    std::map<std::string, SourceToSessionLink> m_sources;
};

} } // ApiGear::ObjectLink

