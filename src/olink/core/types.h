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

#include "olink_common.h"
#include "nlohmann/json.hpp"
#include <string>

namespace ApiGear { namespace ObjectLink {

class OLINK_EXPORT Name {
public:
    // calc.Demo/add
    static std::string getInterfaceId(const std::string& memberId);
    static std::string getMemberName(const std::string& memberId);
    static bool isMemberId(const std::string& id);
    static std::string createMemberId(const std::string& interfaceId, const std::string& memberName);
};

enum class MsgType : int
{
    Link = 10,
    Init = 11,
    Unlink = 12,
    SetProperty = 20,
    PropertyChange = 21,
    Invoke = 30,
    InvokeReply = 31,
    Signal = 40,
    Error = 99,
};

std::string toString(MsgType type);


enum MessageFormat
{
    JSON = 1,
    BSON = 2,
    MSGPACK = 3,
    CBOR = 4,
};

class OLINK_EXPORT MessageConverter {
public:
    MessageConverter(MessageFormat format);
    void setMessageFormat(MessageFormat format);
    nlohmann::json fromString(const std::string& message);
    std::string toString(const nlohmann::json& j);
private:
    MessageFormat m_format;
};


class OLINK_EXPORT IMessageHandler
{
public:
    virtual ~IMessageHandler();
    virtual void handleMessage(const std::string& message) = 0;
};

enum LogLevel {
    Info,
    Debug,
    Warning,
    Error
};

class OLINK_EXPORT ILogger
{
public:
    virtual ~ILogger();
    virtual void writeLog(LogLevel level, std::string message) = 0;
};

typedef std::function<void(LogLevel level, std::string msg)> WriteLogFunc;

typedef std::function<void(std::string msg)> WriteMessageFunc;


class OLINK_EXPORT LoopbackWriter {
public:
    LoopbackWriter(IMessageHandler* handler=nullptr);
    void writeMessage(nlohmann::json j);
    WriteMessageFunc& writeFunc();
private:
    IMessageHandler *m_handler;
    MessageConverter m_converter;
    WriteMessageFunc m_writeFunc;
};



class OLINK_EXPORT InvokeReplyArg {
public:
    std::string name;
    nlohmann::json value;
};

typedef std::function<void(InvokeReplyArg)> InvokeReplyFunc;


class OLINK_EXPORT Base {
public:
    Base();
    virtual ~Base();
    void onLog(WriteLogFunc func);
    void emitLog(LogLevel level, std::string msg);
private:
    WriteLogFunc m_logFunc;
};

} } // ApiGear::ObjectLink
