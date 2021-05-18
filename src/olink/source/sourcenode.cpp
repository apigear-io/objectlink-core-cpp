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

#include "sourcenode.h"
#include "sourceregistry.h"
namespace ApiGear { namespace ObjectLink {


ObjectSourceNode::ObjectSourceNode(std::string name)
    : BaseNode(name)
{

}


ObjectSourceRegistry *ObjectSourceNode::registry()
{
    ObjectSourceRegistry *node = ObjectSourceRegistry::registry(name());
    if(!node) {
        emitLog(LogLevel::Warning, "unknown node: " + name());
        return nullptr;
    }
    return node;
}

void ObjectSourceNode::handleLink(std::string name)
{
    if(!registry()) { return; }
    emitLog(LogLevel::Info, "handleLink name: " + name);
    registry()->attachNode(name, this);
    IObjectSource* s = registry()->getSource(name);
    if(s) {
        s->linked(name, this);
        json props = s->collectProperties();
        emitWrite(ObjectLinkProtocol::initMessage(name, props));
    } else {
        emitLog(LogLevel::Warning, "no source to link: " + name);
    }

}

void ObjectSourceNode::handleUnlink(std::string name)
{
    if(!registry()) { return; }
    IObjectSource* s = registry()->getSource(name);
    if(s) {
        s->unlinked(name);
    }
    registry()->detachNode(name, this);
}

void ObjectSourceNode::handleSetProperty(std::string name, json value)
{
    if(!registry()) { return; }
    IObjectSource* s = registry()->getSource(name);
    if(s) {
        s->setProperty(name, value);
    }
}

void ObjectSourceNode::handleInvoke(int requestId, std::string name, json args)
{
    if(!registry()) { return; }
    IObjectSource* s = registry()->getSource(name);
    if(s) {
        json value = s->invoke(name, args);
        emitWrite(ObjectLinkProtocol::invokeReplyMessage(requestId, name, value));
    }
}

void ObjectSourceNode::notifyPropertyChange(std::string name, json value)
{
    if(!registry()) { return; }
    std::list<ObjectSourceNode *> services = registry()->getNodes(name);
    for(ObjectSourceNode* service: services) {
        service->emitWrite(ObjectLinkProtocol::propertyChangeMessage(name, value));
    }
}

void ObjectSourceNode::notifySignal(std::string name, json args)
{
    if(!registry()) { return; }
    std::list<ObjectSourceNode *> services = registry()->getNodes(name);
    for(ObjectSourceNode* service: services) {
        service->emitWrite(ObjectLinkProtocol::signalMessage(name, args));
    }
}

} } // Apigear::ObjectLink
