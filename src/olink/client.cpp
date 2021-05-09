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

#include "client.h"

#include "protocol.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {

void ObjectLinkClientRegistry::addObject(std::string name, IClientObjectListener *handler)
{
    std::string resource = name.substr(0, name.find("/"));
    m_objects[resource] = handler;
}

void ObjectLinkClientRegistry::removeObject(std::string name)
{
    std::string resource = name.substr(0, name.find("/"));
    m_objects.erase(resource);
}

IClientObjectListener *ObjectLinkClientRegistry::objectListener(std::string name)
{
    std::string resource = name.substr(0, name.find("/"));
    return m_objects[resource];
}

ObjectLinkClient::ObjectLinkClient(IMessageWriter *writer, ILogger *log, MessageFormat format)
    : m_protocol(new Protocol(this, writer, format, log))
    , m_registry(new ObjectLinkClientRegistry())
    , m_log(log)
    , m_nextId(0)
{
}

ObjectLinkClient::~ObjectLinkClient()
{
}

void ObjectLinkClient::addObject(std::string name, IClientObjectListener *handler)
{
    m_registry->addObject(name, handler);
}

void ObjectLinkClient::removeObject(std::string name)
{
    m_registry->removeObject(name);
}

IClientObjectListener *ObjectLinkClient::objectListener(std::string name)
{
    return m_registry->objectListener(name);
}


void ObjectLinkClient::handleLink(std::string name)
{
    std::cout << __func__ << "not implemented" << name;
}

void ObjectLinkClient::handleUnlink(std::string name)
{
    std::cout << __func__ << "not implemented" << name;
}

void ObjectLinkClient::handleInit(std::string name, json props)
{
    IClientObjectListener *l = objectListener(name);
    if(l) {
        l->onInit(name, props);
    }
}

void ObjectLinkClient::handleSetProperty(std::string name, json value)
{
    std::cout << __func__ << "not implemented" << name << value.dump();
}

void ObjectLinkClient::handlePropertyChange(std::string name, json value)
{
    IClientObjectListener *l = objectListener(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void ObjectLinkClient::handleInvoke(int requestId, std::string name, json args)
{
    std::cout << __func__ << "not implemented" << requestId << name << args.dump();
}

void ObjectLinkClient::handleInvokeReply(int requestId, std::string name, json value)
{
    std::cout << "invoke reply" << requestId << name << value.dump();
}

void ObjectLinkClient::handleSignal(std::string name, json args)
{
    IClientObjectListener *l = objectListener(name);
    if(l) {
        l->onSignal(name, args);
    }
}

void ObjectLinkClient::handleError(int msgType, int requestId, std::string error)
{
    std::cout << "error: " << msgType << requestId << error;
}

void ObjectLinkClient::handleMessage(std::string message)
{
    m_protocol->handleMessage(message);
}



} } // Apigear::ObjectLink


