#pragma once

#include "sourcetypes.h"
#include "objectnode.h"


namespace ApiGear { namespace ObjectLink {

class SourceLink;

// registry for remote objects
class SourceNode: public ObjectNode {
private:
public:
    SourceNode(std::string name);
    virtual ~SourceNode() override;
    void addObjectSource(std::string name, IObjectSource* source);
    void removeObjectSource(std::string name);
    IObjectSource* getObjectSource(std::string name);

    void attachSourceLink(std::string name, SourceLink* link);
    void detachSourceLink(std::string name, SourceLink *link);
    std::list<SourceLink*> getSourceLinks(std::string name);

    SourceNode *getSourceNode(std::string name);
private:
    std::map<std::string, SourceToLinksEntry> m_sourceEntries;
};

class SourceNodeManager {
public:
    void setSourceNode(std::string name, SourceNode* node);
    void unsetSourceNode(std::string name);
    static SourceNodeManager& get();
    SourceNode* getSourceNode(std::string name="");
private:
    std::map<std::string, SourceNode*> m_nodes;
};

} } // ApiGear::ObjectLink

