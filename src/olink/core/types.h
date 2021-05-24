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
#include <list>

#include "nlohmann/json.hpp"

namespace ApiGear { namespace ObjectLink {

using json = nlohmann::json;

class Name {
public:
    // calc.Demo/add
    static std::string resourceFromName(std::string name);
    static std::string pathFromName(std::string name);
    static bool hasPath(std::string name);
    static std::string createName(std::string resource, std::string path);
};

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


enum MessageFormat
{
    JSON = 1,
    BSON = 2,
    MSGPACK = 3,
    CBOR = 4,
};

class MessageConverter {
public:
    MessageConverter(MessageFormat format);
    void setMessageFormat(MessageFormat format);
    json fromString(std::string message);
    std::string toString(json j);
private:
    MessageFormat m_format;
};


class IMessageHandler
{
public:
    virtual ~IMessageHandler();
    virtual void handleMessage(std::string message) = 0;
};

enum LogLevel {
    Info,
    Debug,
    Warning,
    Error
};

class ILogger
{
public:
    virtual ~ILogger();
    virtual void log(LogLevel level, std::string message) = 0;
};

typedef std::function<void(LogLevel level, std::string msg)> WriteLogFunc;

typedef std::function<void(std::string msg)> WriteMessageFunc;


class LoopbackWriter {
public:
    LoopbackWriter(IMessageHandler* handler=nullptr);
    void writeMessage(json j);
    WriteMessageFunc& writeFunc();
private:
    IMessageHandler *m_handler;
    MessageConverter m_converter;
    WriteMessageFunc m_writeFunc;
};



class InvokeReplyArg {
public:
    std::string name;
    json value;
};

typedef std::function<void(InvokeReplyArg)> InvokeReplyFunc;


class Base {
public:
    Base();
    virtual ~Base();
    void onLog(WriteLogFunc func);
    void emitLog(LogLevel level, std::string msg);
private:
    WriteLogFunc m_logFunc;
};

} } // ApiGear::ObjectLink
