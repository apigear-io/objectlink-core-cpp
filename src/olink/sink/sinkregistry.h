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
#include "sinktypes.h"
#include "olink/shared/baseregistry.h"

namespace ApiGear { namespace ObjectLink {

class ObjectSinkNode;

class ObjectSinkRegistry : public BaseRegistry
{
public:
    ObjectSinkRegistry(std::string name);
    virtual ~ObjectSinkRegistry();

    void addObjectSink(std::string name, IObjectSink *sink);
    void removeObjectSink(std::string name);
    IObjectSink* getObjectSink(std::string name);

    void setNode(std::string name, ObjectSinkNode* node);
    void unsetNode(ObjectSinkNode *node);
    IObjectSinkNode* getNode(std::string name);

    static ObjectSinkRegistry* registry(std::string name);
private:
    std::map<std::string, SinkToNodeEntry> m_entries;
};

class SinkRegistryManager {
public:
    void setRegistry(std::string name, ObjectSinkRegistry* registry);
    void unsetRegistry(std::string name);
    static SinkRegistryManager& get();
    ObjectSinkRegistry* getRegistry(std::string name);
private:
    std::map<std::string, ObjectSinkRegistry*> m_registries;
};

} } // Apigear::ObjectLink
