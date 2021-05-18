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

#include "sinknode.h"
#include "sinklink.h"

#include "iostream"

namespace ApiGear { namespace ObjectLink {

SinkNode::SinkNode(std::string name)
    : ObjectNode(name)
{
    SinkNodeManager::get().setSinkNode(name, this);
}

SinkNode::~SinkNode()
{
    SinkNodeManager::get().unsetSinkNode(nodeName());
}

void SinkNode::unsetSinkLink(SinkLink *link)
{
    std::list<std::string> resources;
    for(auto& it : m_sinkEntries) {
        if(it.second.link == link) {
            it.second.link = nullptr;
        }
    }
}

void SinkNode::setSinkLink(std::string name, SinkLink *link)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.linkClientToSink: " + resource + ", " + name);
    if(m_sinkEntries.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToLinkEntry entry;
        entry.sink = nullptr;
        entry.link = link;
        m_sinkEntries[resource] = entry;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToLinkEntry& entry = m_sinkEntries[resource];
        entry.link = link;
    }
}



void SinkNode::addObjectSink(std::string name, IObjectSink *sink)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "ObjectLinkSinkRegistry.addObjectSink: " + resource + ", " + name);
    if(m_sinkEntries.count(resource) == 0) {
        emitLog(LogLevel::Info, "new link");
        SinkToLinkEntry link;
        link.sink = sink;
        link.link = nullptr;
        m_sinkEntries[resource] = link;
    } else {
        emitLog(LogLevel::Info, "update link");
        SinkToLinkEntry& link = m_sinkEntries[resource];
        link.sink = sink;
    }
}

void SinkNode::removeObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    if(m_sinkEntries.count(resource) == 0) {
        const SinkToLinkEntry& link = m_sinkEntries[resource];
        if(link.sink) {
            link.sink->onRelease();
        }
    }
    m_sinkEntries.erase(resource);
}

IObjectSink *SinkNode::getObjectSink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object sink" + name + resource);
    if(m_sinkEntries.count(resource) == 0) {
        std::cout << "sink not added: " +  name;
        return nullptr;
    }
    return m_sinkEntries[resource].sink;
}

ISinkLink *SinkNode::getSinkLink(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "object client" + name + resource);
    if(m_sinkEntries.count(resource) == 0) {
        emitLog(LogLevel::Info, "client not linked yet: " + name);
        return nullptr;
    }
    return m_sinkEntries[resource].link;
}

SinkNode *SinkNode::getSinkNode(std::string name)
{
    return SinkNodeManager::get().getSinkNode(name);
}

void SinkNodeManager::setSinkNode(std::string name, SinkNode *registry)
{
    if(m_registries.count(name) == 0) {
        m_registries[name] = registry;
    } else {
        std::cout << "error: " << "registry exits" << name;
    }
}

void SinkNodeManager::unsetSinkNode(std::string name)
{
    if(m_registries.count(name) == 1) {
        m_registries.erase(name);
    } else {
        std::cout << "error: " << "can not unset registry which does not exits" << name;

    }
}

SinkNodeManager &SinkNodeManager::get()
{
    static SinkNodeManager m;
    return m;
}

SinkNode *SinkNodeManager::getSinkNode(std::string name)
{
    if(m_registries.count(name) == 1) {
        return m_registries[name];
    }
    return nullptr;
}



} } // Apigear::ObjectLink
