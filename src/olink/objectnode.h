#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {

class ObjectNode {
public:
    ObjectNode(std::string name);
    virtual ~ObjectNode();
    void onLog(WriteLogFunc func);
    void emitLog(LogLevel level, std::string msg);
    std::string nodeName() const;
private:
    static void registerNode(std::string name, ObjectNode* node);
    static void deregisterNode(std::string name);
public:
    static ObjectNode* node(std::string name);

private:
    std::string m_nodeName;
    WriteLogFunc m_logFunc;
    static std::map<std::string, ObjectNode*> m_nodes;
};


//class ObjectNodeManager {
//public:
//    void setNode(std::string name, ObjectNode* node);
//    void unsetNode(std::string name);
//    static ObjectNodeManager& get();

//private:
//    std::map<std::string, ObjectNode*> m_nodes;
//};

} } // ApiGear::ObjectLink
