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
#include "types.h"

#include <string>
#include <map>
#include <list>
#include "spdlog/spdlog.h"

namespace ApiGear { namespace ObjectLink {

std::string Name::resourceFromName(std::string name)
{
    return name.substr(0, name.find("/"));
}

std::string Name::pathFromName(std::string name)
{
    return name.substr(name.find("/")+1);
}

bool Name::hasPath(std::string name)
{
    return name.find("/") != -1;
}

std::string toString(MessageType type) {
    static std::map<MessageType, std::string> typeNames = {
        { MessageType::LINK, "link" },
        { MessageType::UNLINK, "unlink" },
        { MessageType::INIT, "init" },
        { MessageType::SET_PROPERTY, "property_change" },
        { MessageType::PROPERTY_CHANGE, "signal_property_change" },
        { MessageType::INVOKE, "invole" },
        { MessageType::INVOKE_REPLY, "invoke_reply" },
        { MessageType::SIGNAL, "signal" },
        { MessageType::ERROR, "error"

        },
    };
    auto result = typeNames.find(type);
    if (result == typeNames.end()) {
        return std::string("unknown");
    }

    return result->second;
}

IMessageWriter::~IMessageWriter() {}
IMessageHandler::~IMessageHandler() {}
ILogger::~ILogger() {}


LoopbackWriter::LoopbackWriter(IMessageHandler *handler)
    : m_handler(handler)
{
}

void LoopbackWriter::writeMessage(std::string message) {
    if(!m_handler) {
        spdlog::warn("error: LoopBackWriter no handler yet set. skip");
        return;
    }
    SPDLOG_TRACE("handle message {}", message);
    m_handler->handleMessage(message);
}

void LoopbackWriter::setHandler(IMessageHandler *handler) { m_handler = handler; }



} } // ApiGear::ObjectLink

