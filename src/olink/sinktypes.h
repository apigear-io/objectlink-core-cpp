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

#include "nlohmann/json.hpp"

namespace ApiGear { namespace ObjectLink {

using json = nlohmann::json;

class IObjectLinkSink
{
public:
    virtual ~IObjectLinkSink() {}
    virtual void onSignal(std::string name, json args) = 0;
    virtual void onPropertyChanged(std::string name, json value) = 0;
    virtual void onInit(std::string name, json props) = 0;
};

class IObjectLinkClient {
public:
    ~IObjectLinkClient() {}
    virtual void invoke(std::string name, json args) = 0;
    virtual void link(std::string name) = 0;
    virtual void unlink(std::string name) = 0;
};

} } // Apigear::ObjectLink

