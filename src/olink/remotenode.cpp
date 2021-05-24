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

#include "remotenode.h"

namespace ApiGear { namespace ObjectLink {


// ********************************************************************
// IRemoteNode
// ********************************************************************

IRemoteNode::~IRemoteNode()
{
}

// ********************************************************************
// IObjectSource
// ********************************************************************

IObjectSource::~IObjectSource()
{
}

// ********************************************************************
// RemoteRegistry
// ********************************************************************

RemoteRegistry::RemoteRegistry()
    : Base()
{
}

RemoteRegistry &RemoteRegistry::get()
{
    static RemoteRegistry r;
    return r;
}

void RemoteRegistry::addObjectSource(IObjectSource *source)
{
    std::string resource = Name::resourceFromName(source->olinkObjectName());
    emitLog(LogLevel::Info, "RemoteRegistry.addObjectSource: " + resource);
    m_entries[resource].source = source;
}

void RemoteRegistry::removeObjectSource(IObjectSource *source)
{
    std::string resource = Name::resourceFromName(source->olinkObjectName());
    emitLog(LogLevel::Info, "RemoteRegistry.removeObjectSource: " + resource);
    m_entries.erase(resource);
}

IObjectSource *RemoteRegistry::getObjectSource(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "RemoteRegistry.getObjectSource: " + resource);
    return m_entries[resource].source;
}

std::set<RemoteNode *> RemoteRegistry::getRemoteNodes(std::string name)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "RemoteRegistry.getRemoteNodes: " + resource);
    return m_entries[resource].nodes;
}

void RemoteRegistry::attachRemoteNode(RemoteNode *node)
{
    emitLog(LogLevel::Info, "RemoteRegistry.attachRemoteNode");
}

void RemoteRegistry::detachRemoteNode(RemoteNode *node)
{
    emitLog(LogLevel::Info, "RemoteRegistry.detachRemoteNode");
    for(auto &entry: m_entries) {
        if(entry.second.nodes.count(node) != 0) {
            entry.second.nodes.erase(node);
        }
    }
}

void RemoteRegistry::linkRemoteNode(std::string name, RemoteNode *node)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "RemoteRegistry.linkRemoteNode: " + resource);
    m_entries[resource].nodes.insert(node);
}

void RemoteRegistry::unlinkRemoteNode(std::string name, RemoteNode *node)
{
    std::string resource = Name::resourceFromName(name);
    emitLog(LogLevel::Info, "RemoteRegistry.unlinkRemoteNode: " + resource);
    m_entries[resource].nodes.erase(node);
}


// ********************************************************************
// RemoteNode
// ********************************************************************

RemoteNode::RemoteNode()
    : BaseNode()
{
    registry().attachRemoteNode(this);
}

RemoteNode::~RemoteNode()
{
    registry().detachRemoteNode(this);
}

IObjectSource *RemoteNode::getObjectSource(std::string name)
{
    return registry().getObjectSource(name);
}

void RemoteNode::addObjectSource(IObjectSource *source)
{
    RemoteRegistry::get().addObjectSource(source);
}

void RemoteNode::removeObjectSource(IObjectSource *source)
{
    RemoteRegistry::get().removeObjectSource(source);
}


void RemoteNode::handleLink(std::string name)
{
    emitLog(LogLevel::Info, "handleLink name: " + name);
    IObjectSource* s = getObjectSource(name);
    if(s) {
        registry().linkRemoteNode(name, this);
        s->olinkLinked(name, this);
        json props = s->olinkCollectProperties();
        emitWrite(Protocol::initMessage(name, props));
    } else {
        emitLog(LogLevel::Warning, "no source to link: " + name);
    }

}

void RemoteNode::handleUnlink(std::string name)
{
    IObjectSource* s = getObjectSource(name);
    if(s) {
        registry().unlinkRemoteNode(name, this);
        s->olinkUnlinked(name);
    }
}

void RemoteNode::handleSetProperty(std::string name, json value)
{
    IObjectSource* s = getObjectSource(name);
    if(s) {
        s->olinkSetProperty(name, value);
    }
}

void RemoteNode::handleInvoke(int requestId, std::string name, json args)
{
    IObjectSource* s = getObjectSource(name);
    if(s) {
        json value = s->olinkInvoke(name, args);
        emitWrite(Protocol::invokeReplyMessage(requestId, name, value));
    }
}

void RemoteNode::notifyPropertyChange(std::string name, json value)
{
    for(auto node: registry().getRemoteNodes(name)) {
        node->emitWrite(Protocol::propertyChangeMessage(name, value));
    }
}

void RemoteNode::notifySignal(std::string name, json args)
{
    for(auto node: registry().getRemoteNodes(name)) {
        node->emitWrite(Protocol::signalMessage(name, args));
    }
}

RemoteRegistry &RemoteNode::registry()
{
    return RemoteRegistry::get();
}

void RemoteNode::linkNode(std::string name)
{
    registry().linkRemoteNode(name, this);
}

void RemoteNode::unlinkNode(std::string name)
{
    registry().unlinkRemoteNode(name, this);
}

} } // Apigear::ObjectLink
