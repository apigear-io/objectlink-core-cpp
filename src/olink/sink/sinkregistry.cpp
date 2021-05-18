/*
* MIT License
*
* Copyright (c) 2021 ApiGear
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "sinkregistry.h"
#include "sinknode.h"

#include "iostream"

namespace ApiGear { namespace ObjectLink {

ObjectSinkRegistry::ObjectSinkRegistry(std::string name)
    : BaseRegistry(name)
{
    SinkRegistryManager::get().setRegistry(name, this);
}

ObjectSinkRegistry::~ObjectSinkRegistry()
{
    SinkRegistryManager::get().unsetRegistry(name());
}

void ObjectSinkRegistry::unsetNode(ObjectSinkNode *node)
{
    std::list<std::string> resources;
    for(auto& it : m_entries) {
        if(it.second.node == node) {
            it.second.node = nullptr;
        }
    }
}

void ObjectSinkRegistry::setNode(std::string name, ObjectSinkNode *node)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.linkClientToSink: " + resource + ", " + name);
    if(m_entries.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToNodeEntry entry;
        entry.sink = nullptr;
        entry.node = node;
        m_entries[resource] = entry;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToNodeEntry& entry = m_entries[resource];
        entry.node = node;
    }
}



void ObjectSinkRegistry::addObjectSink(std::string name, IObjectSink *sink)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.addObjectSink: " + resource + ", " + name);
    if(m_entries.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToNodeEntry link;
        link.sink = sink;
        link.node = nullptr;
        m_entries[resource] = link;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToNodeEntry& link = m_entries[resource];
        link.sink = sink;
    }
}

void ObjectSinkRegistry::removeObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_entries.count(resource) == 0) {
        const SinkToNodeEntry& link = m_entries[resource];
        if(link.sink) {
            link.sink->onRelease();
        }
    }
    m_entries.erase(resource);
}

IObjectSink *ObjectSinkRegistry::getObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object sink" + name + resource);
    if(m_entries.count(resource) == 0) {
        std::cout << "sink not added: " +  name;
        return nullptr;
    }
    return m_entries[resource].sink;
}

IObjectSinkNode *ObjectSinkRegistry::getNode(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object client" + name + resource);
    if(m_entries.count(resource) == 0) {
        emitLog(LogLevel::Info, "client not linked yet: " + name);
        return nullptr;
    }
    return m_entries[resource].node;
}

ObjectSinkRegistry *ObjectSinkRegistry::registry(std::string name)
{
    return SinkRegistryManager::get().getRegistry(name);
}

void SinkRegistryManager::setRegistry(std::string name, ObjectSinkRegistry *registry)
{
    if(m_registries.count(name) == 0) {
        m_registries[name] = registry;
    } else {
        std::cout << "error: " << "registry exits" << name;
    }
}

void SinkRegistryManager::unsetRegistry(std::string name)
{
    if(m_registries.count(name) == 1) {
        m_registries.erase(name);
    } else {
        std::cout << "error: " << "can not unset registry which does not exits" << name;

    }
}

SinkRegistryManager &SinkRegistryManager::get()
{
    static SinkRegistryManager m;
    return m;
}

ObjectSinkRegistry *SinkRegistryManager::getRegistry(std::string name)
{
    if(m_registries.count(name) == 1) {
        return m_registries[name];
    }
    return nullptr;
}



} } // Apigear::ObjectLink
