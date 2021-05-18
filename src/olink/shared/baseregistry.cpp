#include "baseregistry.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {

BaseRegistry::BaseRegistry(std::string name)
    : m_nodeName(name)
    , m_logFunc(nullptr)
{
}

BaseRegistry::~BaseRegistry()
{
}

void BaseRegistry::onLog(WriteLogFunc func)
{
    m_logFunc = func;
}

void BaseRegistry::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }

}

std::string BaseRegistry::name() const
{
    return m_nodeName;
}


//void ObjectNodeManager::setNode(std::string name, ObjectNode *node)
//{
//    if(m_nodes.count(name) == 0) {
//        m_nodes[name] = node;
//    } else {
//        std::cout << "error: " << "registry exits" << name;
//    }
//}

//void ObjectNodeManager::unsetNode(std::string name)
//{
//    if(m_nodes.count(name) == 1) {
//        m_nodes.erase(name);
//    } else {
//        std::cout << "error: " << "can not unset registry which does not exits" << name;

//    }
//}

//ObjectNodeManager &ObjectNodeManager::get()
//{
//    static ObjectNodeManager m;
//    return m;
//}

//ObjectNode *ObjectNodeManager::node(std::string name)
//{
//    if(m_nodes.count(name) == 1) {
//        return m_nodes[name];
//    }
//    return nullptr;
//}


} } // ApiGear::ObjectLink
