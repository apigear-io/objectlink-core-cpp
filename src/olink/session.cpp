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

#include "session.h"
#include "sourceregistry.h"
#include "sinkregistry.h"
#include "core/messages.h"
#include "core/types.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


ObjectLinkSession::ObjectLinkSession(IMessageWriter *writer, MessageFormat format, ILogger *log)
    : m_sourceRegistry(new ObjectLinkSourceRegistry())
    , m_sinkRegistry(new ObjectLinkSinkRegistry())
    , m_protocol(new ObjectLinkProtocol(this, writer, format, log))
    , m_log(log)
{
}

ObjectLinkSession::~ObjectLinkSession()
{
    delete m_sourceRegistry;
    m_sourceRegistry = nullptr;
    delete m_sinkRegistry;
    m_sinkRegistry = nullptr;
    delete m_protocol;
    m_protocol = nullptr;
}

void ObjectLinkSession::addObjectSource(std::string name, IObjectLinkSource *listener)
{
    std::cout << __func__ << name;
    return m_sourceRegistry->addObjectSource(name, listener);
}

void ObjectLinkSession::removeObjectSource(std::string name)
{
    std::cout << __func__ << name;
    return m_sourceRegistry->removeObjectSource(name);
}

IObjectLinkSource *ObjectLinkSession::objectSource(std::string name)
{
    std::cout << __func__ << "name";
    return m_sourceRegistry->objectSource(name);
}

void ObjectLinkSession::addObjectSink(std::string name, IObjectLinkSink *handler)
{
    m_sinkRegistry->addObjectSink(name, handler);
}

void ObjectLinkSession::removeObjectSink(std::string name)
{
    m_sinkRegistry->removeObjectSink(name);
}

IObjectLinkSink *ObjectLinkSession::objectSink(std::string name)
{
    return m_sinkRegistry->objectSink(name);
}

ObjectLinkProtocol *ObjectLinkSession::protocol() const
{
    return m_protocol;
}

ObjectLinkSourceRegistry *ObjectLinkSession::sourceRegistry() const
{
    return m_sourceRegistry;
}

ObjectLinkSinkRegistry *ObjectLinkSession::sinkRegistry() const
{
    return m_sinkRegistry;
}

void ObjectLinkSession::invoke(std::string name, json args)
{
    protocol()->writeInvoke(name, args);
}

void ObjectLinkSession::setProperty(std::string name, json value)
{
    protocol()->writeSetProperty(name, value);
}

void ObjectLinkSession::handleLink(std::string name)
{
    m_sourceRegistry->linkSource(name, this);
    IObjectLinkSource* s = objectSource(name);
    if(s) {
        s->linked(name, this);
        json props = s->collectProperties();
        protocol()->writeInit(name, props);
    }
}

void ObjectLinkSession::handleUnlink(std::string name)
{
    IObjectLinkSource* s = objectSource(name);
    if(s) {
        s->unlinked(name);
    }
    m_sourceRegistry->unlinkSource(name, this);
}

void ObjectLinkSession::handleInit(std::string name, json props)
{
    IObjectLinkSink *s = objectSink(name);
    if(s) {
        s->onInit(name, props);
    }
}

void ObjectLinkSession::handleSetProperty(std::string name, json value)
{
    IObjectLinkSource* s = objectSource(name);
    if(s) {
        s->setProperty(name, value);
    }
 }

void ObjectLinkSession::handlePropertyChange(std::string name, json value)
{
    IObjectLinkSink *l = objectSink(name);
    if(l) {
        l->onPropertyChanged(name, value);
    }
}

void ObjectLinkSession::handleInvoke(int requestId, std::string name, json args)
{
    IObjectLinkSource* s = objectSource(name);
    if(s) {
        json value = s->invoke(name, args);
        protocol()->writeInvokeReply(requestId, name, value);
    }
}

void ObjectLinkSession::handleInvokeReply(int requestId, std::string name, json value)
{
    std::cout << __func__ << "not implemented" << requestId << name << value;
}

void ObjectLinkSession::handleSignal(std::string name, json args)
{
    IObjectLinkSink *s = objectSink(name);
    if(s) {
        s->onSignal(name, args);
    }
}

void ObjectLinkSession::handleError(int msgType, int requestId, std::string error)
{
    std::cout << __func__ << "not implemented" << msgType << requestId << error;
}

void ObjectLinkSession::notifyPropertyChange(std::string name, json value)
{
    std::list<ObjectLinkSession *> sessions = m_sourceRegistry->objectServices(name);
    for(const ObjectLinkSession* session: sessions) {
        session->protocol()->writePropertyChange(name, value);
    }
}

void ObjectLinkSession::notifySignal(std::string name, json args)
{
    std::list<ObjectLinkSession *> sessions = m_sourceRegistry->objectServices(name);
    for(const ObjectLinkSession* session: sessions) {
        session->protocol()->writeSignal(name, args);
    }
}


} } // Apigear::ObjectLink






