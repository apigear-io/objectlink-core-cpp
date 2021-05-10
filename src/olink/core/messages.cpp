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

#include "messages.h"
#include "nlohmann/json.hpp"


using json = nlohmann::json;

namespace ApiGear { namespace ObjectLink {


Message::Message()
{

}

std::string Message::resourceFromName(std::string name)
{
    return name.substr(0, name.find("/"));
}

std::string Message::pathFromName(std::string name)
{
    return name.substr(name.find("/"));
}

json Message::linkMessage(std::string name)
{
    return json::array(
                { MessageType::LINK, name }
                );
}

json Message::unlinkMessage(std::string name)
{
    return json::array(
                { MessageType::UNLINK, name }
                );
}

json Message::initMessage(std::string name, json props)
{
    return json::array(
                { MessageType::INIT, name, props }
                );
}

json Message::setPropertyMessage(std::string name, json value)
{
    return json::array(
                { MessageType::SET_PROPERTY, name, value }
                );

}

json Message::propertyChangeMessage(std::string name, json value)
{
    return json::array(
                { MessageType::PROPERTY_CHANGE, name, value }
                );
}

json Message::invokeMessage(int requestId, std::string name, json args)
{
    return json::array(
                { MessageType::INVOKE, requestId, name, args }
                );
}

json Message::invokeReplyMessage(int requestId, std::string name, json value)
{
    return json::array(
                { MessageType::INVOKE_REPLY, requestId, name, value }
                );
}

json Message::signalMessage(std::string name, json args)
{
    return json::array(
                { MessageType::SIGNAL, name, args }
                );
}

json Message::errorMessage(int msgType, int requestId, std::string error)
{
    return json::array(
                { MessageType::ERROR, msgType, requestId, error }
                );
}


} } // ApiGear::ObjectLink

