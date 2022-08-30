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
#include "loopbackwriter.h"

namespace ApiGear {
namespace ObjectLink {


LoopbackWriter::LoopbackWriter(IMessageHandler* handler)
    : m_handler(handler)
    , m_converter(MessageFormat::JSON)
{
    m_writeFunc = [this](nlohmann::json j) {
        std::string data = m_converter.toString(j);
        if (m_handler) {
            m_handler->handleMessage(data);
        }
    };
}

void LoopbackWriter::writeMessage(nlohmann::json j) {
    m_writeFunc(j.dump());
}

WriteMessageFunc& LoopbackWriter::writeFunc() {
    return m_writeFunc;
}

} } // ApiGear::ObjectLink
