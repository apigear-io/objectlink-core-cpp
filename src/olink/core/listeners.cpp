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
#include "listeners.h"
#include <iostream>

namespace ApiGear { namespace ObjectLink {


IObjectLinkProtocolListener::~IObjectLinkProtocolListener() {}

void DefaultObjectLinkProtocolListener::handleLink(std::string name)
{
    std::cout << "not implemented " << __func__ << name << std::endl;
}

void DefaultObjectLinkProtocolListener::handleUnlink(std::string name)
{
    std::cout << "not implemented " << __func__ << name << std::endl;
}

void DefaultObjectLinkProtocolListener::handleInvoke(int requestId, std::string name, json args)
{
    std::cout << "not implemented " << __func__ << requestId << name << args.dump() << std::endl;
}

void DefaultObjectLinkProtocolListener::handleSetProperty(std::string name, json value)
{
    std::cout << "not implemented " << __func__ << name << value.dump() << std::endl;
}


void DefaultObjectLinkProtocolListener::handleInit(std::string name, json props)
{
    std::cout << "not implemented " << __func__ << name << props.dump() << std::endl;
}

void DefaultObjectLinkProtocolListener::handleInvokeReply(int requestId, std::string name, json value)
{
    std::cout << "not implemented " << __func__ << requestId << name << value.dump() << std::endl;
}

void DefaultObjectLinkProtocolListener::handleSignal(std::string name, json args)
{
    std::cout << "not implemented " << __func__ << name << args.dump()  << std::endl;
}

void DefaultObjectLinkProtocolListener::handlePropertyChange(std::string name, json value)
{
    std::cout << "not implemented " << __func__ << name << value.dump() << std::endl;
}

void DefaultObjectLinkProtocolListener::handleError(int msgType, int requestId, std::string error)
{
    std::cout << "not implemented " << __func__ << msgType << requestId << error << std::endl;
}

} } // ApiGear::ObjectLink

