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

namespace ApiGear {
namespace ObjectLink {


struct OLinkContent
{
    //std::string content;
    nlohmann::json content;
};

struct InitialProperty
{
    std::string propertyName;
    OLinkContent propertyValue;
};

template<typename ArgType>
InitialProperty toInitialProperty(const std::string& name, const ArgType& arg)
{
    InitialProperty prop;
    prop.propertyName = name;
    prop.propertyValue.content = nlohmann::json(arg);
    return prop;
}

template<typename ValueType>
void readValue(InitialProperty& property, ValueType& value)
{
    value = property.propertyValue.content.get<ValueType>();
}

OLINK_EXPORT void from_json(const nlohmann::json& j, InitialProperty& p);
OLINK_EXPORT void to_json(nlohmann::json& j, const InitialProperty& p);
OLINK_EXPORT bool operator==(const OLinkContent& lhs, const OLinkContent& rhs);

// If content always come as array this is not needed
template<typename ValueType>
OLinkContent invokeReturnValue(const ValueType& value)
{
    OLinkContent content;
    content.content = nlohmann::json(value);
    return content;
}


// If content always come as array this is not needed
// Use to write single value for: setProperty, notifyProperty
// see argumentsToContent for writing arguments - even if a single one.
template<typename ValueType>
OLinkContent propertyToContent(ValueType value)
{
    OLinkContent content;
    content.content = nlohmann::json(value);
    return content;
}

// last call of recursive fillContent(nlohmann::json::array& content_array, T const& first, Parameters const&... rest)
OLINK_EXPORT void fillContent(nlohmann::json& content_array, size_t current);

template <typename T, typename... Parameters>
void fillContent(nlohmann::json& content_array, size_t current, const T& first, const Parameters&... rest)
{
    content_array[current] = first;
    current++;
    fillContent(content_array, current, rest...);
}

// Read if single value, for setProperty, notifyProperty, invokeResponse
template<typename ValueType>
void readValue(const OLinkContent& content, ValueType& value)
{
    value = content.content.get<ValueType>();
}

// Use to write arguments for: notifySignal, invoke, even if there is a single argument
template<typename ... Parameters>
OLinkContent argumentsToContent(const Parameters&  ...inputs)
{
    OLinkContent content;
    content.content = nlohmann::json::array();
    auto inputs_size = sizeof...(inputs);
    content.content.get_ptr<nlohmann::json::array_t*>()->reserve(inputs_size);
    size_t current = 0;
    fillContent(content.content, current, inputs...);
    return content;
}


// Use to read for arguments: notifySignal, invoke
struct OLinContentStreamReader
{
    OLinContentStreamReader(const OLinkContent& content)
        :m_content(content)
    {}

    template<typename ArgType>
    void read(ArgType& arg)
    {
        arg = m_content.content[currentIndex].get<ArgType>();
        currentIndex++;
    }
    size_t argumentsCount()
    {
        return  m_content.content.size();
    }

private:
    const OLinkContent& m_content;
    size_t currentIndex = 0;
};

OLINK_EXPORT std::string toString(const OLinkContent& content);


}} //namespace ApiGear::ObjectLink