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
#pragma once

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace ApiGear { namespace ObjectLink {

typedef unsigned long long Id;

enum class MessageType : int
{
    LINK = 10,
    INIT = 11,
    UNLINK = 12,
    SET_PROPERTY = 20,
    PROPERTY_CHANGE = 21,
    INVOKE = 30,
    INVOKE_REPLY = 31,
    SIGNAL = 40,
    ERROR = 99,
};

std::string toString(MessageType type);


enum class MessageFormat : int
{
    JSON = 1,
    BSON = 2,
    MSGPACK = 3,
    CBOR = 4,
};

class IMessageWriter
{
public:
    virtual ~IMessageWriter() {}
    virtual void writeMessage(std::string message) = 0;
};


class IMessageHandler
{
public:
    virtual ~IMessageHandler() {}
    virtual void handleMessage(std::string message) = 0;
};

class IClientListener {
    virtual ~IClientListener() {}

};

class IServiceListener {
    virtual ~IServiceListener() {}
};


class ILogger
{
public:
    virtual ~ILogger() {}
    virtual void info(std::string message) = 0;
    virtual void debug(std::string message) = 0;
    virtual void warning(std::string message) = 0;
    virtual void error(std::string message) = 0;
};


} } // ApiGear::ObjectLink
