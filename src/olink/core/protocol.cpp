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

#include <vector>

using namespace std;

namespace ApiGear { namespace ObjectLink {

template<typename T>
T getSafeValue(json j)
{
    return ( j.is_object() || j.is_array()) ? j.get<T>() : T();
}

ObjectLinkProtocol::ObjectLinkProtocol(IProtocolListener *listener, IMessageWriter *writer, MessageFormat format, ILogger *log)
    : m_listener(listener)
    , m_writer(writer)
    , m_format(format)
    , m_log(log)
    , m_nextId(0)
{
    assert(m_writer);
    assert(m_log);
    assert(m_listener);
}

int ObjectLinkProtocol::nextId()
{
    m_nextId++;
    return m_nextId;
}



void ObjectLinkProtocol::handleMessage(std::string message)
{
    json j = fromString(message);
    m_log->debug("handle message: " +  j.dump());
    if(!j.is_array()) {
        m_log->warning("message must be array");
        return;
    }
    const int msgType = j[0].get<int>();
    switch(msgType) {
    case int(MessageType::LINK): {
        const string name = j[1].get<string>();
        listener()->handleLink(name);
        break;
    }
    case int(MessageType::INIT): {
        const string name = j[1].get<string>();
        const json props = j[2].get<json>();
        listener()->handleInit(name, props);
        break;
    }
    case int(MessageType::UNLINK): {
        const string name = j[1].get<string>();
        listener()->handleUnlink(name);
        break;
    }
    case int(MessageType::SET_PROPERTY): {
        const string name = j[1].get<string>();
        const json value = j[2].get<json>();
        listener()->handleSetProperty(name, value);
        break;
    }
    case int(MessageType::PROPERTY_CHANGE): {
        const string name = j[1].get<string>();
        const json value = j[2].get<json>();
        listener()->handlePropertyChange(name, value);
        break;
    }
    case int(MessageType::INVOKE): {
        const int id = j[1].get<int>();
        const string name = j[2].get<string>();
        const json args = j[3].get<json>();
        listener()->handleInvoke(id, name, args);
        break;
    }
    case int(MessageType::INVOKE_REPLY): {
        const int id = j[1].get<int>();
        const string name = j[2].get<string>();
        const json value = j[3].get<json>();
        listener()->handleInvokeReply(id, name, value);
        break;
    }
    case int(MessageType::SIGNAL): {
        const string name = j[1].get<string>();
        const json args = j[2].get<json>();
        listener()->handleSignal(name, args);
        break;
    }
    case int(MessageType::ERROR): {
        const int msgType = j[1].get<int>();
        const int requestId = j[2].get<int>();
        const string error = j[3].get<string>();
        listener()->handleError(msgType, requestId, error);
        break;
    }
    default:
        m_log->warning("message not support " + message);
    }
}

void ObjectLinkProtocol::writeMessage(json j)
{
    assert(m_writer);
    string message = toString(j);
    m_log->debug("write message " + message);
    if(m_writer) {
        m_writer->writeMessage(message);
    } else {
        m_log->warning("no write set, can not send");
    }
}

IProtocolListener *ObjectLinkProtocol::listener() const
{
    assert(m_listener);
    return m_listener;
}

void ObjectLinkProtocol::writeLink(std::string name)
{
    json msg = Message::linkMessage(name);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeUnlink(std::string name)
{
    json msg = Message::unlinkMessage(name);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeInit(std::string name, json props)
{
    json msg = Message::initMessage(name, props);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeSetProperty(std::string name, json value)
{
    json msg = Message::setPropertyMessage(name, value);
    writeMessage(msg);
}

void ObjectLinkProtocol::writePropertyChange(std::string name, json value)
{
    json msg = Message::propertyChangeMessage(name, value);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeInvoke(std::string name, json args)
{
    int requestId = nextId();
    json msg = Message::invokeMessage(requestId, name, args);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeInvokeReply(int requestId, std::string name, json value)
{
    json msg = Message::invokeReplyMessage(requestId, name, value);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeSignal(std::string name, json args)
{
    json msg = Message::signalMessage(name, args);
    writeMessage(msg);
}

void ObjectLinkProtocol::writeError(int msgType, int requestId, std::string name)
{
    json msg = Message::errorMessage(msgType, requestId, name);
    writeMessage(msg);
}





json ObjectLinkProtocol::fromString(string message)
{
    switch(m_format) {
    case MessageFormat::JSON:
        return json::parse(message);
    case MessageFormat::BSON:
        return json::from_bson(message);
    case MessageFormat::MSGPACK:
        return json::from_msgpack(message);
    case MessageFormat::CBOR:
        return json::from_cbor(message);
    }

    return json();
}

string ObjectLinkProtocol::toString(json j)
{
    vector<uint8_t> v;
    switch(m_format) {
    case MessageFormat::JSON:
        return j.dump();
    case MessageFormat::BSON:
        v = json::to_bson(j);
        return string(v.begin(), v.end());
    case MessageFormat::MSGPACK:
        v = json::to_msgpack(j);
        return string(v.begin(), v.end());
    case MessageFormat::CBOR:
        v = json::to_cbor(j);
        return string(v.begin(), v.end());
    }

    return string();
}

} } // ApiGear::ObjectLink
