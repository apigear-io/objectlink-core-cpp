#include "objectnode.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


std::map<std::string, ObjectNode*> ObjectNode::m_nodes;

ObjectNode::ObjectNode(std::string name)
    : m_nodeName(name)
    , m_logFunc(nullptr)
{
    ObjectNode::registerNode(name, this);
}

ObjectNode::~ObjectNode()
{
    ObjectNode::deregisterNode(nodeName());
}

void ObjectNode::onLog(WriteLogFunc func)
{
    m_logFunc = func;
}

void ObjectNode::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }

}

std::string ObjectNode::nodeName() const
{
    return m_nodeName;
}

void ObjectNode::registerNode(std::string name, ObjectNode *node)
{
    if(m_nodes.count(name) == 0) {
        m_nodes[name] = node;
    } else {
        std::cout << "error: " << "registry exits" << name;
    }
}

void ObjectNode::deregisterNode(std::string name)
{
    if(m_nodes.count(name) == 1) {
        m_nodes.erase(name);
    } else {
        std::cout << "error: " << "can not unset registry which does not exits" << name;

    }
}

ObjectNode *ObjectNode::node(std::string name)
{
    if(m_nodes.count(name) == 1) {
        return m_nodes[name];
    }
    return nullptr;
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
