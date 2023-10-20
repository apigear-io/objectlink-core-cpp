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

namespace 
{
    OLinkMessage createMessage(MsgType msgType, const std::string& id)
    {
        OLinkMessage message;
        message.message = nlohmann::json::array({ msgType, id });
        return message;
    }

    OLinkMessage createMessage(MsgType msgType, const std::string& id, const OLinkContent& props)
    {
        OLinkMessage message;
        message.message = nlohmann::json::array({ msgType, id, props.content });
        return message;
    }

    OLinkMessage createMessage(MsgType msgType, int callId, const std::string& id, const OLinkContent& props)
    {
        OLinkMessage message;
        message.message = nlohmann::json::array({ msgType, callId, id, props.content });
        return message;
    }

    OLinkMessage createErrorMessage(MsgType msgType, int id, const std::string& errorMsg)
    {
        OLinkMessage message;
        message.message = nlohmann::json::array({ MsgType::Error, msgType, id, errorMsg });
        return message;
    }
}

OLinkMessage Protocol::linkMessage(const std::string& objectId)
{
    return createMessage(MsgType::Link, objectId);
}

OLinkMessage Protocol::unlinkMessage(const std::string& objectId)
{
    return createMessage(MsgType::Unlink, objectId);
}

OLinkMessage Protocol::initMessage(const std::string& objectId, const OLinkContent& props)
{
    return createMessage(MsgType::Init, objectId, props);
}

OLinkMessage Protocol::setPropertyMessage(const std::string& propertyId, const OLinkContent& value)
{
    return createMessage(MsgType::SetProperty, propertyId, value);
}

OLinkMessage Protocol::propertyChangeMessage(const std::string& propertyId, const OLinkContent& value)
{
    return createMessage(MsgType::PropertyChange, propertyId, value);
}

OLinkMessage Protocol::invokeMessage(int requestId, const std::string& methodId, const OLinkContent& args)
{
    return createMessage(MsgType::Invoke, requestId, methodId, args);
}

OLinkMessage Protocol::invokeReplyMessage(int requestId, const std::string& methodId, const OLinkContent& value)
{
    return createMessage(MsgType::InvokeReply, requestId, methodId, value);
}

OLinkMessage Protocol::signalMessage(const std::string& signalId , const OLinkContent& args)
{
    return createMessage(MsgType::Signal, signalId, args);
}

OLinkMessage Protocol::errorMessage(MsgType msgType, int requestId, const std::string& error)
{
    return createErrorMessage(msgType, requestId, error);
}

bool Protocol::handleMessage(const OLinkMessage& message, IProtocolListener& listener) {

    m_lastError = "";
    OLinkMessageStreamReader msgReader(message);
    auto isValid = msgReader.validate(m_lastError);
    if(!isValid) {
        return false;
    }
    MsgType msgType;
    msgReader.read(msgType);
    switch(msgType) {
    case MsgType::Link: {
        std::string objectId;
        msgReader.read(objectId);
        listener.handleLink(objectId);
        break;
    }
    case MsgType::Init: {
        std::string objectId;
        OLinkContent props;
        msgReader.read(objectId);
        msgReader.read(props);
        listener.handleInit(objectId, props);
        break;
    }
    case MsgType::Unlink: {
        std::string objectId;
        msgReader.read(objectId);
        listener.handleUnlink(objectId);
        break;
    }
    case MsgType::SetProperty: {
        std::string propertyId;
        OLinkContent value;
        msgReader.read(propertyId);
        msgReader.read(value);
        listener.handleSetProperty(propertyId, value);
        break;
    }
    case MsgType::PropertyChange: {
        std::string propertyId;
        OLinkContent value;
        msgReader.read(propertyId);
        msgReader.read(value);
        listener.handlePropertyChange(propertyId, value);
        break;
    }
    case MsgType::Invoke: {
        int callId;
        std::string methodId;
        OLinkContent args;
        msgReader.read(callId);
        msgReader.read(methodId);
        msgReader.read(args);
        listener.handleInvoke(callId, methodId, args);
        break;
    }
    case MsgType::InvokeReply: {
        int callId;
        std::string methodId;
        OLinkContent value;
        msgReader.read(callId);
        msgReader.read(methodId);
        msgReader.read(value);
        listener.handleInvokeReply(callId, methodId, value);
        break;
    }
    case MsgType::Signal: {
        std::string signalId;
        OLinkContent args;
        msgReader.read(signalId);
        msgReader.read(args);
        listener.handleSignal(signalId, args);
        break;
    }
    case MsgType::Error: {
        int msgTypeErr;
        int requestId;
        std::string error;
        msgReader.read(msgTypeErr);
        msgReader.read(requestId);
        msgReader.read(error);
        listener.handleError(msgTypeErr, requestId, error);
        break;
    }
    default:
        m_lastError = "message not supported: " + msgReader.toString();
        return false;
    }
    return true;
}

std::string Protocol::lastError()
{
    return m_lastError;
}

} } // ApiGear::ObjectLink


