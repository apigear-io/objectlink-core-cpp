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

#include "service.h"
#include "core/protocol.h"
#include "core/types.h"

namespace ApiGear { namespace ObjectLink {


ServiceIO::ServiceIO(SourceRegistry *registry)
    : m_registry(registry)
    , m_protocol(this)
    , m_converter(MessageFormat::JSON)
    , m_writeFunc(nullptr)
    , m_logFunc(nullptr)
{

}

void ServiceIO::onWrite(WriteMessageFunc func)
{
    m_writeFunc = func;
}

void ServiceIO::emitWrite(json j)
{
    std::string data = m_converter.toString(j);
    if(m_writeFunc) {
        m_writeFunc(data);
    }
}


void ServiceIO::onLog(LogWriterFunc func) {
    m_logFunc = func;
}

void ServiceIO::emitLog(LogLevel level, std::string msg) {
    if(m_logFunc) {
        m_logFunc(level, msg);
    }
}
void ServiceIO::handleMessage(std::string data)
{
    const json& j = m_converter.fromString(data);
    m_protocol.handleMessage(j);
}

void ServiceIO::handleLink(std::string name)
{
    if(!m_registry) { return; }
    emitLog(LogLevel::Info, "handleLink name: " + name);
    m_registry->linkSource(name, this);
    ISource* s = m_registry->objectSource(name);
    if(s) {
        s->linked(name, this);
        json props = s->collectProperties();
        emitWrite(Protocol::initMessage(name, props));
    } else {
        emitLog(LogLevel::Warning, "no source to link: " + name);
    }

}

void ServiceIO::handleUnlink(std::string name)
{
    if(!m_registry) { return; }
    ISource* s = m_registry->objectSource(name);
    if(s) {
        s->unlinked(name);
    }
    m_registry->unlinkSource(name, this);
}

void ServiceIO::handleSetProperty(std::string name, json value)
{
    if(!m_registry) { return; }
    ISource* s = m_registry->objectSource(name);
    if(s) {
        s->setProperty(name, value);
    }
}

void ServiceIO::handleInvoke(int requestId, std::string name, json args)
{
    if(!m_registry) { return; }
    ISource* s = m_registry->objectSource(name);
    if(s) {
        json value = s->invoke(name, args);
        emitWrite(Protocol::invokeReplyMessage(requestId, name, value));
    }
}

void ServiceIO::notifyPropertyChange(std::string name, json value)
{
    if(!m_registry) { return; }
    std::list<IServiceIO *> services = m_registry->objectServices(name);
    for(IServiceIO* service: services) {
        service->emitWrite(Protocol::propertyChangeMessage(name, value));
    }
}

void ServiceIO::notifySignal(std::string name, json args)
{
    if(!m_registry) { return; }
    std::list<IServiceIO *> services = m_registry->objectServices(name);
    for(IServiceIO* service: services) {
        service->emitWrite(Protocol::signalMessage(name, args));
    }
}

//void AbstractNode::writeSignal(std::string name, json args)
//{
//    json msg = Messages::signalMessage(name, args);
//    writeMessage(msg);
//}


//AbstractNode::AbstractNode(MessageFormat format)
//    : m_message(this)
//    , m_converter(format)
//{
//}

//AbstractNode::~AbstractNode()
//{
//}

//void AbstractNode::setMessageFormat(MessageFormat format)
//{
//    m_converter.setMessageFormat(format);
//}

//void AbstractNode::onLog(LogWriterFunc func)
//{
//    m_logFunc = func;
//}

//void AbstractNode::log(LogLevel level, std::string msg)
//{
//    if(m_logFunc) {
//        m_logFunc(level, msg);
//    }
//}

//void AbstractNode::handleMessage(std::string message)
//{
//    const json& j = m_converter.fromString(message);
//    m_message.handleMessage(j);
//}

//void AbstractNode::invoke(std::string name, json args, InvokeReplyFunc func)
//{
//    writeInvoke(name, args, func);
//}

//void AbstractNode::setProperty(std::string name, json value)
//{
//    writeSetProperty(name, value);
//}

//void AbstractNode::handleLink(std::string name)
//{
//    log(LogLevel::Info, "handleLink name: " + name);
//    sourceRegistry().linkSource(name, this);
//    IObjectLinkSource* s = sourceRegistry().objectSource(name);
//    if(s) {
//        s->linked(name, this);
//        json props = s->collectProperties();
//        writeInit(name, props);
//    } else {
//        log(LogLevel::Warning, "no source to link: " + name);
//    }
//}

//void AbstractNode::handleUnlink(std::string name)
//{
//    IObjectLinkSource* s = sourceRegistry().objectSource(name);
//    if(s) {
//        s->unlinked(name);
//    }
//    sourceRegistry().unlinkSource(name, this);
//}

//void AbstractNode::handleInit(std::string name, json props)
//{
//    log(LogLevel::Info, "handle init" +  name + props.dump());
//    IObjectLinkSink *s = sinkRegistry().objectSink(name);
//    if(s) {
//        s->onInit(name, props, this);
//    }
//}

//void AbstractNode::handleSetProperty(std::string name, json value)
//{
//    IObjectLinkSource* s = sourceRegistry().objectSource(name);
//    if(s) {
//        s->setProperty(name, value);
//    }
// }

//void AbstractNode::handlePropertyChange(std::string name, json value)
//{
//    IObjectLinkSink *l = sinkRegistry().objectSink(name);
//    if(l) {
//        l->onPropertyChanged(name, value);
//    }
//}

//void AbstractNode::handleInvoke(int requestId, std::string name, json args)
//{
//    IObjectLinkSource* s = sourceRegistry().objectSource(name);
//    if(s) {
//        json value = s->invoke(name, args);
//        writeInvokeReply(requestId, name, value);
//    }
//}

//void AbstractNode::handleInvokeReply(int requestId, std::string name, json value)
//{
//    log(LogLevel::Info, "handleInvokeReply not implemented " + std::to_string(requestId) + name + value.dump());
//}

//void AbstractNode::handleSignal(std::string name, json args)
//{
//    IObjectLinkSink *s = sinkRegistry().objectSink(name);
//    if(s) {
//        s->onSignal(name, args);
//    }
//}

//void AbstractNode::handleError(int msgType, int requestId, std::string error)
//{
//    log(LogLevel::Info, "handleError not implemented " + std::to_string(msgType) + std::to_string(requestId) + error);
//}

//void AbstractNode::notifyPropertyChange(std::string name, json value)
//{
//    std::list<AbstractNode *> sessions = sourceRegistry().objectServices(name);
//    for(AbstractNode* session: sessions) {
//        session->writePropertyChange(name, value);
//    }
//}

//void AbstractNode::notifySignal(std::string name, json args)
//{
//    std::list<AbstractNode *> sessions = sourceRegistry().objectServices(name);
//    for(AbstractNode* session: sessions) {
//        session->writeSignal(name, args);
//    }
//}

//void AbstractNode::writeMessage(json j)
//{
//    const std::string& message = m_converter.toString(j);
//    log(LogLevel::Debug, "writeMessage " + j.dump());
//    if(m_writeFunc) {
//        m_writeFunc(message);
//    } else {
//        log(LogLevel::Warning, "no writer set, can not write");
//    }
//}

//void AbstractNode::writeLink(std::string name)
//{
//    json msg = Messages::linkMessage(name);
//    writeMessage(msg);
//}

//void AbstractNode::writeUnlink(std::string name)
//{
//    json msg = Messages::unlinkMessage(name);
//    writeMessage(msg);
//}

//void AbstractNode::writeInit(std::string name, json props)
//{
//    json msg = Messages::initMessage(name, props);
//    writeMessage(msg);
//}

//void AbstractNode::writeSetProperty(std::string name, json value)
//{
//    json msg = Messages::setPropertyMessage(name, value);
//    writeMessage(msg);
//}

//void AbstractNode::writePropertyChange(std::string name, json value)
//{
//    json msg = Messages::propertyChangeMessage(name, value);
//    writeMessage(msg);
//}

//void AbstractNode::writeInvoke(std::string name, json args, InvokeReplyFunc func)
//{
//    int requestId = nextId();
//    m_invokesPending[requestId] = func;
//    json msg = Messages::invokeMessage(requestId, name, args);
//    writeMessage(msg);
//}

//void AbstractNode::writeInvokeReply(int requestId, std::string name, json value)
//{
//    json msg = Messages::invokeReplyMessage(requestId, name, value);
//    writeMessage(msg);
//}

//void AbstractNode::writeSignal(std::string name, json args)
//{
//    json msg = Messages::signalMessage(name, args);
//    writeMessage(msg);
//}

//void AbstractNode::writeError(MessageType msgType, int requestId, std::string name)
//{
//    json msg = Messages::errorMessage(msgType, requestId, name);
//    writeMessage(msg);
//}


} } // Apigear::ObjectLink
