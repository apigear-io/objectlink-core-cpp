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
#include "objectnode.h"

namespace ApiGear { namespace ObjectLink {

class SinkLink;

class SinkNode : public ObjectNode
{
public:
    SinkNode(std::string name);
    virtual ~SinkNode();
    void addObjectSink(std::string name, IObjectSink *sink);
    void removeObjectSink(std::string name);
    IObjectSink* getObjectSink(std::string name);

    void setSinkLink(std::string name, SinkLink* link);
    void unsetSinkLink(SinkLink *link);
    ISinkLink* getSinkLink(std::string name);

    static SinkNode* getSinkNode(std::string name);
private:
    std::map<std::string, SinkToLinkEntry> m_sinkEntries;
};

class SinkNodeManager {
public:
    void setSinkNode(std::string name, SinkNode* registry);
    void unsetSinkNode(std::string name);
    static SinkNodeManager& get();
    SinkNode* getSinkNode(std::string name);
private:
    std::map<std::string, SinkNode*> m_registries;
};

} } // Apigear::ObjectLink
