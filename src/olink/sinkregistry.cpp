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

#include "iostream"

namespace ApiGear { namespace ObjectLink {

SinkRegistry::SinkRegistry(std::string name)
    : m_name(name)
    , m_logFunc(nullptr)
{
    SinkRegistryManager::get().setRegistry(name, this);
}

SinkRegistry::~SinkRegistry()
{
    SinkRegistryManager::get().unsetRegistry(name());
}

void SinkRegistry::onLog(LogWriterFunc func)
{
    m_logFunc = func;
}

void SinkRegistry::emitLog(LogLevel level, std::string msg)
{
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}

std::string SinkRegistry::name() const
{
    return m_name;
}

void SinkRegistry::unlinkClient(IClient *client)
{
    std::list<std::string> resources;
    for(auto& it : m_links) {
        if(it.second.client == client) {
            it.second.client = nullptr;
        }
    }
}

ISink* SinkRegistry::linkSinkToClient(std::string name, IClient *client)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.linkClientToSink: " + resource + ", " + name);
    if(m_links.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToClientLink link;
        link.sink = nullptr;
        link.client = client;
        m_links[resource] = link;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToClientLink& link = m_links[resource];
        link.client = client;
        return link.sink;
    }
    return nullptr;
}



IClient* SinkRegistry::addObjectSink(std::string name, ISink *sink)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.addObjectSink: " + resource + ", " + name);
    if(m_links.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToClientLink link;
        link.sink = sink;
        link.client = nullptr;
        m_links[resource] = link;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToClientLink& link = m_links[resource];
        link.sink = sink;
        return link.client;
    }
    return nullptr;
}

void SinkRegistry::removeObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_links.count(resource) == 0) {
        const SinkToClientLink& link = m_links[resource];
        if(link.sink) {
            link.sink->onRelease();
        }
    }
    m_links.erase(resource);
}

ISink *SinkRegistry::objectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object sink" + name + resource);
    if(m_links.count(resource) == 0) {
        std::cout << "sink not added: " +  name;
        return nullptr;
    }
    return m_links[resource].sink;
}

IClient *SinkRegistry::objectClient(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object client" + name + resource);
    if(m_links.count(resource) == 0) {
        emitLog(LogLevel::Info, "client not linked yet: " + name);
        return nullptr;
    }
    return m_links[resource].client;
}

void SinkRegistryManager::setRegistry(std::string name, SinkRegistry *registry)
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

SinkRegistry *SinkRegistryManager::registry(std::string name)
{
    if(m_registries.count(name) == 1) {
        return m_registries[name];
    }
    return nullptr;
}



} } // Apigear::ObjectLink
