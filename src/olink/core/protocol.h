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

#include "types.h"
#include "listeners.h"
#include <string>
#include "nlohmann/json.hpp"


namespace ApiGear { namespace ObjectLink {

using json = nlohmann::json;


//class Connection : public IMessageHandler
//{
//public:
//    Connection(IProtocolListener *listener, IMessageWriter *writer, MessageFormat format, ILogger *log);
//    void setListener(IProtocolListener *listener);
//    void setWriter(IMessageWriter *writer);
//    void setMessageFormat(MessageFormat format);
//    int nextId();
//    void handleMessage(std::string message) override;
//    void onWrite(WriteMessageFunc func);
//    void writeMessage(json j);
//    json fromString(std::string message);
//    std::string toString(json j);
//    IProtocolListener *listener() const;
//    void writeLink(std::string name);
//    void writeUnlink(std::string name);
//    void writeInit(std::string name, json props);
//    void writeSetProperty(std::string name, json value);
//    void writePropertyChange(std::string name, json value);
//    void writeInvoke(std::string name, json args, InvokeReplyFunc func);
//    void writeInvokeReply(int requestId, std::string name, json value);
//    void writeSignal(std::string name, json args);
//    void writeError(MessageType msgType, int requestId, std::string name);
//    void handleInvokeReply(int requestId, std::string name, json value);

//private:
//    IProtocolListener *m_listener;
//    IMessageWriter *m_writer;
//    MessageFormat m_format;
//    ILogger *m_log;
//    int m_nextId;
//    std::map<int,InvokeReplyFunc> m_invokesPending;
//    WriteMessageFunc m_writeFunc;
//};

} } // ApiGear::ObjectLink

