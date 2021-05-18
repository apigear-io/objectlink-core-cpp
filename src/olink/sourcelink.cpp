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

#include "sourcelink.h"
#include "core/protocol.h"
#include "core/types.h"

namespace ApiGear { namespace ObjectLink {


SourceLink::SourceLink(std::string name)
    : ObjectLink(name)
{

}


SourceNode *SourceLink::sourceNode()
{
    SourceNode *node = SourceNodeManager::get().getSourceNode(nodeName());
    if(!node) {
        emitLog(LogLevel::Warning, "unknown node: " + nodeName());
        return nullptr;
    }
    return node;
}

void SourceLink::handleLink(std::string name)
{
    if(!sourceNode()) { return; }
    emitLog(LogLevel::Info, "handleLink name: " + name);
    sourceNode()->attachSourceLink(name, this);
    IObjectSource* s = sourceNode()->getObjectSource(name);
    if(s) {
        s->linked(name, this);
        json props = s->collectProperties();
        emitWrite(Protocol::initMessage(name, props));
    } else {
        emitLog(LogLevel::Warning, "no source to link: " + name);
    }

}

void SourceLink::handleUnlink(std::string name)
{
    if(!sourceNode()) { return; }
    IObjectSource* s = sourceNode()->getObjectSource(name);
    if(s) {
        s->unlinked(name);
    }
    sourceNode()->detachSourceLink(name, this);
}

void SourceLink::handleSetProperty(std::string name, json value)
{
    if(!sourceNode()) { return; }
    IObjectSource* s = sourceNode()->getObjectSource(name);
    if(s) {
        s->setProperty(name, value);
    }
}

void SourceLink::handleInvoke(int requestId, std::string name, json args)
{
    if(!sourceNode()) { return; }
    IObjectSource* s = sourceNode()->getObjectSource(name);
    if(s) {
        json value = s->invoke(name, args);
        emitWrite(Protocol::invokeReplyMessage(requestId, name, value));
    }
}

void SourceLink::notifyPropertyChange(std::string name, json value)
{
    if(!sourceNode()) { return; }
    std::list<SourceLink *> services = sourceNode()->getSourceLinks(name);
    for(SourceLink* service: services) {
        service->emitWrite(Protocol::propertyChangeMessage(name, value));
    }
}

void SourceLink::notifySignal(std::string name, json args)
{
    if(!sourceNode()) { return; }
    std::list<SourceLink *> services = sourceNode()->getSourceLinks(name);
    for(SourceLink* service: services) {
        service->emitWrite(Protocol::signalMessage(name, args));
    }
}

} } // Apigear::ObjectLink
