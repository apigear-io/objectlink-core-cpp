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
#include "protocol.h"
#include "messages.h"
#include "types.h"

namespace ApiGear { namespace ObjectLink {

template<typename T>
T getSafeValue(json j)
{
    return ( j.is_object() || j.is_array()) ? j.get<T>() : T();
}

//Connection::Connection(IProtocolListener *listener, IMessageWriter *writer, MessageFormat format, ILogger *log)
//    : m_listener(nullptr)
//    , m_writer(nullptr)
//    , m_format(MessageFormat::JSON)
//    , m_log(nullptr)
//    , m_nextId(0)
//    , m_writeFunc(nullptr)
//{
//}

//void Connection::setListener(IProtocolListener *listener)
//{
//    m_listener = listener;
//}

//void Connection::setWriter(IMessageWriter *writer)
//{
//    m_writer = writer;
//}

//int Connection::nextId()
//{
//    m_nextId++;
//    return m_nextId;
//}


//void Connection::handleMessage(std::string message)
//{
//    m_log->info("Protocol::handleMessage" + message);
//    json j = fromString(message);

//    if(!j.is_array()) {
//        m_log->warning("message must be array");
//        return;
//    }
//    const int msgType = j[0].get<int>();
//    switch(msgType) {
//    case int(MessageType::LINK): {
//        const std::string name = j[1].get<std::string>();
//        listener()->handleLink(name);
//        break;
//    }
//    case int(MessageType::INIT): {
//        const std::string name = j[1].get<std::string>();
//        const json props = j[2].get<json>();
//        if(listener()) listener()->handleInit(name, props);
//        break;
//    }
//    case int(MessageType::UNLINK): {
//        const std::string name = j[1].get<std::string>();
//        if(listener()) listener()->handleUnlink(name);
//        break;
//    }
//    case int(MessageType::SET_PROPERTY): {
//        const std::string name = j[1].get<std::string>();
//        const json value = j[2].get<json>();
//        if(listener()) listener()->handleSetProperty(name, value);
//        break;
//    }
//    case int(MessageType::PROPERTY_CHANGE): {
//        const std::string name = j[1].get<std::string>();
//        const json value = j[2].get<json>();
//        if(listener()) listener()->handlePropertyChange(name, value);
//        break;
//    }
//    case int(MessageType::INVOKE): {
//        const int id = j[1].get<int>();
//        const std::string name = j[2].get<std::string>();
//        const json args = j[3].get<json>();
//        if(listener()) listener()->handleInvoke(id, name, args);
//        break;
//    }
//    case int(MessageType::INVOKE_REPLY): {
//        const int id = j[1].get<int>();
//        const std::string name = j[2].get<std::string>();
//        const json value = j[3].get<json>();
//        handleInvokeReply(id, name, value);
//        listener()->handleInvokeReply(id, name, value);
//        break;
//    }
//    case int(MessageType::SIGNAL): {
//        const std::string name = j[1].get<std::string>();
//        const json args = j[2].get<json>();
//        listener()->handleSignal(name, args);
//        break;
//    }
//    case int(MessageType::ERROR): {
//        const int msgType = j[1].get<int>();
//        const int requestId = j[2].get<int>();
//        const std::string error = j[3].get<std::string>();
//        listener()->handleError(msgType, requestId, error);
//        break;
//    }
//    default:
//        m_log->warning("message not supported " + message);
//    }
//}

//void Connection::onWrite(WriteMessageFunc func)
//{
//    m_writeFunc = func;
//}

//void Connection::writeMessage(json j)
//{
//    assert(m_writer);
//    std::string message = toString(j);
//    m_log->debug("writeMessage " + j.dump());
//    if(m_writeFunc) {
//        m_writeFunc(message);
//    }
//    if(m_writer) {
//        m_writer->writeMessage(message);
//    } else {
//        m_log->warning("no write set, can not send");
//    }
//}

//IProtocolListener *Connection::listener() const
//{
//    assert(m_listener);
//    return m_listener;
//}

//void Connection::writeLink(std::string name)
//{
//    json msg = Messages::linkMessage(name);
//    writeMessage(msg);
//}

//void Connection::writeUnlink(std::string name)
//{
//    json msg = Messages::unlinkMessage(name);
//    writeMessage(msg);
//}

//void Connection::writeInit(std::string name, json props)
//{
//    json msg = Messages::initMessage(name, props);
//    writeMessage(msg);
//}

//void Connection::writeSetProperty(std::string name, json value)
//{
//    json msg = Messages::setPropertyMessage(name, value);
//    writeMessage(msg);
//}

//void Connection::writePropertyChange(std::string name, json value)
//{
//    json msg = Messages::propertyChangeMessage(name, value);
//    writeMessage(msg);
//}

//void Connection::writeInvoke(std::string name, json args, InvokeReplyFunc func)
//{
//    int requestId = nextId();
//    m_invokesPending[requestId] = func;
//    json msg = Messages::invokeMessage(requestId, name, args);
//    writeMessage(msg);
//}

//void Connection::writeInvokeReply(int requestId, std::string name, json value)
//{
//    json msg = Messages::invokeReplyMessage(requestId, name, value);
//    writeMessage(msg);
//}

//void Connection::writeSignal(std::string name, json args)
//{
//    json msg = Messages::signalMessage(name, args);
//    writeMessage(msg);
//}

//void Connection::writeError(MessageType msgType, int requestId, std::string name)
//{
//    json msg = Messages::errorMessage(msgType, requestId, name);
//    writeMessage(msg);
//}

//void Connection::handleInvokeReply(int requestId, std::string name, json value)
//{
//    m_log->info("handle invoke reply " + std::to_string(requestId) + name + value.dump());
//    if(m_invokesPending.count(requestId) == 1) {
//        const InvokeReplyFunc& func = m_invokesPending[requestId];
//        const InvokeReplyArg arg{name, value};
//        func(arg);
//        m_invokesPending.erase(requestId);
//    } else {
//        writeError(MessageType::INVOKE, requestId, "no pending invoke " + name + std::to_string(requestId));
//        m_log->warning("no pending invoke " + name + std::to_string(requestId));
//    }
//}

//json Connection::fromString(std::string message)
//{
//    switch(m_format) {
//    case MessageFormat::JSON:
//        return json::parse(message);
//    case MessageFormat::BSON:
//        return json::from_bson(message);
//    case MessageFormat::MSGPACK:
//        return json::from_msgpack(message);
//    case MessageFormat::CBOR:
//        return json::from_cbor(message);
//    }

//    return json();
//}

//std::string Connection::toString(json j)
//{
//    std::vector<uint8_t> v;
//    switch(m_format) {
//    case MessageFormat::JSON:
//        return j.dump();
//    case MessageFormat::BSON:
//        v = json::to_bson(j);
//        return std::string(v.begin(), v.end());
//    case MessageFormat::MSGPACK:
//        v = json::to_msgpack(j);
//        return std::string(v.begin(), v.end());
//    case MessageFormat::CBOR:
//        v = json::to_cbor(j);
//        return std::string(v.begin(), v.end());
//    }

//    return std::string();
//}

} } // ApiGear::ObjectLink
