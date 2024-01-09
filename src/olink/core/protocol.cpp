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
#include "nlohmann/json.hpp"
#include <string>


namespace ApiGear { namespace ObjectLink {



OLinkMessage& Protocol::linkMessage(IMessageWriter& serializer, const std::string& objectId)
{
    serializer.writeNext(MsgType::Link);
    return serializer.writeNext(objectId);
}

OLinkMessage& Protocol::unlinkMessage(IMessageWriter& serializer, const std::string& objectId)
{
    serializer.writeNext(MsgType::Unlink);
    return serializer.writeNext(objectId);
}

OLinkMessage& Protocol::initMessage(IMessageWriter& serializer, const std::string& objectId, const OLinkContent& props)
{
    serializer.writeNext(MsgType::Init);
    serializer.writeNext(objectId);
    return serializer.writeNext(props);
}

OLinkMessage& Protocol::setPropertyMessage(IMessageWriter& serializer, const std::string& propertyId, const OLinkContent& value)
{
    serializer.writeNext(MsgType::SetProperty);
    serializer.writeNext(propertyId);
    return serializer.writeNext(value);
}

OLinkMessage& Protocol::propertyChangeMessage(IMessageWriter& serializer, const std::string& propertyId, const OLinkContent& value)
{
    serializer.writeNext(MsgType::PropertyChange);
    serializer.writeNext(propertyId);
    return serializer.writeNext(value);
}

OLinkMessage& Protocol::invokeMessage(IMessageWriter& serializer, int requestId, const std::string& methodId, const OLinkContent& args)
{
    serializer.writeNext(MsgType::Invoke);
    serializer.writeNext(requestId);
    serializer.writeNext(methodId);
    return serializer.writeNext(args);
}

OLinkMessage& Protocol::invokeReplyMessage(IMessageWriter& serializer, int requestId, const std::string& methodId, const OLinkContent& value)
{
    serializer.writeNext(MsgType::InvokeReply);
    serializer.writeNext(requestId);
    serializer.writeNext(methodId);
    return serializer.writeNext(value);
}

OLinkMessage& Protocol::signalMessage(IMessageWriter& serializer, const std::string& signalId , const OLinkContent& args)
{
    serializer.writeNext(MsgType::Signal);
    serializer.writeNext(signalId);
    return serializer.writeNext(args);
}

OLinkMessage& Protocol::errorMessage(IMessageWriter& serializer, MsgType msgType, int requestId, const std::string& error)
{
    serializer.writeNext(msgType);
    serializer.writeNext(requestId);
    return serializer.writeNext(error);
}

bool Protocol::handleMessage(IMessageReader& deserializer, IProtocolListener& listener)
{
    m_lastError = "";
    auto isValid = deserializer.validate(m_lastError);
    if(!isValid) {
        return false;
    }
    MsgType msgType;
    deserializer.readNext(msgType);
    switch(msgType) {
    case MsgType::Link: {
        std::string objectId = "";
        deserializer.readNext(objectId);
        listener.handleLink(objectId);
        break;
    }
    case MsgType::Init: {
        std::string objectId = "";
        OLinkContent props = {};
        deserializer.readNext(objectId);
        deserializer.readNext(props);
        listener.handleInit(objectId, props);
        break;
    }
    case MsgType::Unlink: {
        std::string objectId = "";
        deserializer.readNext(objectId);
        listener.handleUnlink(objectId);
        break;
    }
    case MsgType::SetProperty: {
        std::string propertyId = "";
        OLinkContent value = {};
        deserializer.readNext(propertyId);
        deserializer.readNext(value);
        listener.handleSetProperty(propertyId, value);
        break;
    }
    case MsgType::PropertyChange: {
        std::string propertyId = "";
        OLinkContent value = {};
        deserializer.readNext(propertyId);
        deserializer.readNext(value);
        listener.handlePropertyChange(propertyId, value);
        break;
    }
    case MsgType::Invoke: {
        int callId = 0;
        std::string methodId = "";
        OLinkContent args = {};
        //TODO DOROTA
        deserializer.readNext(callId);
        deserializer.readNext(methodId);
        deserializer.readNext(args);
        listener.handleInvoke(callId, methodId, args);
        break;
    }
    case MsgType::InvokeReply: {
        int callId = 0;
        std::string methodId = "";
        OLinkContent value = {};
        deserializer.readNext(callId);
        deserializer.readNext(methodId);
        deserializer.readNext(value);
        listener.handleInvokeReply(callId, methodId, value);
        break;
    }
    case MsgType::Signal: {
        std::string signalId = "";
        OLinkContent args = {};
        deserializer.readNext(signalId);
        deserializer.readNext(args);
        listener.handleSignal(signalId, args);
        break;
    }
    case MsgType::Error: {
        MsgType msgTypeErr = MsgType::Error;
        int requestId = 0;
        std::string error = "";
        deserializer.readNext(msgTypeErr);
        deserializer.readNext(requestId);
        deserializer.readNext(error);
        listener.handleError(msgTypeErr, requestId, error);
        break;
    }
    default:
        m_lastError = "message not supported: " + deserializer.getAsString();
        return false;
    }
    return true;
}

std::string Protocol::lastError()
{
    return m_lastError;
}

} } // ApiGear::ObjectLink


