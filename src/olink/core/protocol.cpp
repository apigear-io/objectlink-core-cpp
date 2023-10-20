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

OLinkMessage Protocol::linkMessage(const std::string& objectId)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Link, objectId }
                );
    return message;
}

OLinkMessage Protocol::unlinkMessage(const std::string& objectId)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Unlink, objectId }
                );
    return message;
}

OLinkMessage Protocol::initMessage(const std::string& objectId, const OLinkContent& props)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Init, objectId, props.content }
                );
    return message;
}

OLinkMessage Protocol::setPropertyMessage(const std::string& propertyId, const OLinkContent& value)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::SetProperty, propertyId, value.content }
                );
    return message;
}

OLinkMessage Protocol::propertyChangeMessage(const std::string& propertyId, const OLinkContent& value)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::PropertyChange, propertyId, value.content }
                );
    return message;
}

OLinkMessage Protocol::invokeMessage(int requestId, const std::string& methodId, const OLinkContent& args)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Invoke, requestId, methodId, args.content }
                );
    return message;
}

OLinkMessage Protocol::invokeReplyMessage(int requestId, const std::string& methodId, const OLinkContent& value)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::InvokeReply, requestId, methodId, value.content }
                );
    return message;
}

OLinkMessage Protocol::signalMessage(const std::string& signalId , const OLinkContent& args)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Signal, signalId, args.content }
                );
    return message;
}

OLinkMessage Protocol::errorMessage(MsgType msgType, int requestId, const std::string& error)
{
    OLinkMessage message;
    message.message = nlohmann::json::array(
                { MsgType::Error, msgType, requestId, error }
                );
    return message;
}

bool Protocol::handleMessage(const OLinkMessage& message, IProtocolListener& listener) {

    m_lastError = "";
    auto& data = message.message;
    if(!data.is_array()) {
        m_lastError = "message must be array";
        return false;
    }
    const int msgType = data[0].get<int>();
    switch(msgType) {
    case int(MsgType::Link): {
        const auto& objectId = data[1].get<std::string>();
        listener.handleLink(objectId);
        break;
    }
    case int(MsgType::Init): {
        const auto& objectId = data[1].get<std::string>();
        const auto& props = data[2].get<nlohmann::json>();
        listener.handleInit(objectId, { props });
        break;
    }
    case int(MsgType::Unlink): {
        const auto& objectId = data[1].get<std::string>();
        listener.handleUnlink(objectId);
        break;
    }
    case int(MsgType::SetProperty): {
        const auto& propertyId = data[1].get<std::string>();
        const auto& value = data[2].get<nlohmann::json>();
        listener.handleSetProperty(propertyId, { value });
        break;
    }
    case int(MsgType::PropertyChange): {
        const auto& propertyId = data[1].get<std::string>();
        const auto& value = data[2].get<nlohmann::json>();
        listener.handlePropertyChange(propertyId, { value });
        break;
    }
    case int(MsgType::Invoke): {
        const auto& id = data[1].get<int>();
        const auto& methodId = data[2].get<std::string>();
        const auto& args = data[3].get<nlohmann::json>();
        listener.handleInvoke(id, methodId, { args });
        break;
    }
    case int(MsgType::InvokeReply): {
        const auto& id = data[1].get<int>();
        const auto& methodId = data[2].get<std::string>();
        const auto& value = data[3].get<nlohmann::json>();
        listener.handleInvokeReply(id, methodId, { value });
        break;
    }
    case int(MsgType::Signal): {
        const auto& signalId = data[1].get<std::string>();
        const auto& args = data[2].get<nlohmann::json>();
        listener.handleSignal(signalId, { args });
        break;
    }
    case int(MsgType::Error): {
        const auto& msgTypeErr = data[1].get<int>();
        const auto& requestId = data[2].get<int>();
        const auto& error = data[3].get<std::string>();
        listener.handleError(msgTypeErr, requestId, error);
        break;
    }
    default:
        m_lastError = "message not supported: " + data.dump();
        return false;
    }
    return true;
}

std::string Protocol::lastError()
{
    return m_lastError;
}

} } // ApiGear::ObjectLink


