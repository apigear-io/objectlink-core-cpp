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
#include "nlohmann/json.hpp"


namespace ApiGear { namespace ObjectLink {

using json = nlohmann::json;


class Protocol;

class IClientObjectListener
{
public:
    virtual ~IClientObjectListener() {}
    virtual void onSignal(std::string name, json args) = 0;
    virtual void onPropertyChanged(std::string name, json value) = 0;
    virtual void onInit(std::string name, json props) = 0;
};

class ObjectLinkClientRegistry
{
public:
    void addObject(std::string name, IClientObjectListener* handler);
    void removeObject(std::string name);
    IClientObjectListener* objectListener(std::string name);
private:
    std::map<std::string, IClientObjectListener*> m_objects;
};


class ObjectLinkClient: public IProtocolListener, public IMessageHandler
{
public:
    ObjectLinkClient(IMessageWriter *writer, ILogger *log, MessageFormat format);
    virtual ~ObjectLinkClient();
    void addObject(std::string name, IClientObjectListener* handler);
    void removeObject(std::string name);
    IClientObjectListener* objectListener(std::string name);
    void invoke(std::string name, json args);
    void link(std::string name);
    void unlink(std::string name);


    // IProtocolListener interface
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

    // IMessageHandler interface
public:
    void handleMessage(std::string message) override;
private:
    Protocol* m_protocol;
    ObjectLinkClientRegistry *m_registry;
    ILogger *m_log;
    int m_nextId;

};

} } // Apigear::ObjectLink



