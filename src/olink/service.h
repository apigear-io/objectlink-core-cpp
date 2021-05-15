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

#include "olink/core/types.h"
#include "olink/core/messages.h"
#include "olink/core/listeners.h"
#include "sourceregistry.h"
#include "sinkregistry.h"
#include "sourcetypes.h"
#include "sinktypes.h"

namespace ApiGear { namespace ObjectLink {


class Service: public IService, public IMessagesListener, public IMessageHandler {
public:
    Service(SourceRegistry *registry);
    void onWrite(WriteMessageFunc func);
    void emitWrite(json j) override;
    void onLog(LogWriterFunc func);
    void emitLog(LogLevel level, std::string msg);
    void writePropertyChange(std::string name, json value);

    // IMessagesListener interface
public:
    void handleLink(std::string name) override;
    void handleUnlink(std::string name) override;
    void handleInit(std::string name, json props) override;
    void handleSetProperty(std::string name, json value) override;
    void handlePropertyChange(std::string name, json value) override;
    void handleInvoke(int requestId, std::string name, json args) override;
    void handleInvokeReply(int requestId, std::string name, json value) override;
    void handleSignal(std::string name, json args) override;
    void handleError(int msgType, int requestId, std::string error) override;
    // IObjectLinkService interface
public:
    void notifyPropertyChange(std::string name, json value) override;
    void notifySignal(std::string name, json args) override;
    void handleMessage(std::string data) override;
private:
    SourceRegistry *m_registry;
    ILogger* m_log;
    MessageConverter m_converter;
    Messages m_messages;
    WriteMessageFunc m_writeFunc;
    LogWriterFunc m_logFunc;

};

} } // Apigear::ObjectLink



