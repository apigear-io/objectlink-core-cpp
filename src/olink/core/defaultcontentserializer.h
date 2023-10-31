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
#include "olinkcontent.h"
#include <string>

namespace ApiGear {
namespace ObjectLink {


inline void from_json(const nlohmann::json& j, InitialProperty& p)
{
    p.propertyName = j[0].get<std::string>();
    p.propertyValue.content = j[1];
}
inline void to_json(nlohmann::json& j, const InitialProperty& p)
{
    j = nlohmann::json{ p.propertyName, p.propertyValue.content };
}

// last call of recursive fillContent(nlohmann::json::array& content_array, T const& first, Parameters const&... rest)
inline void fillContent(nlohmann::json& content_array, size_t current)
{
    (void)content_array;
    (void)current;
}

template <typename T, typename... Parameters>
void fillContent(nlohmann::json& content_array, size_t current, const T& first, const Parameters&... rest)
{
    content_array[current] = first;
    current++;
    fillContent(content_array, current, rest...);
}


namespace NlohmannSerializer
{
    template<typename ArgType>
    InitialProperty toInitialProperty(const std::string& name, const ArgType& arg)
    {
        InitialProperty prop;
        prop.propertyName = name;
        prop.propertyValue.content = arg;
        return prop;
    }


    template<typename ValueType>
    void fromInitialProperty(const InitialProperty& input, ValueType& target)
    {
        target = input.propertyValue.content.get<ValueType>();
    }



    namespace Value
    {

        template<typename ArgType>
        OLinkContent serialize(const ArgType& arg)
        {
            OLinkContent output = {};
            output.content = nlohmann::json(arg);
            return output;
        }

        template<typename ValueType>
        void deserialize(const OLinkContent& input, ValueType& target)
        {
            target = input.content.get<ValueType>();
        }


    } //namespace Value

    namespace Arguments
    {
        // Use to write arguments for: notifySignal, invoke, even if there is a single argument
        template<typename ... Parameters>
        OLinkContent serialize(const Parameters&  ...inputs)
        {
            OLinkContent content = {};
            content.content = nlohmann::json::array();
            auto inputs_size = sizeof...(inputs);
            content.content.get_ptr<nlohmann::json::array_t*>()->reserve(inputs_size);
            size_t current = 0;
            fillContent(content.content, current, inputs...);
            return content;
        }

        inline size_t argumentsCount(OLinkContent& content)
        {
            return  content.content.size();
        }

        // Use to read for arguments: notifySignal, invoke
        struct Deserializer
        {
            Deserializer(const OLinkContent& content)
                :m_content(content)
            {}

            template<typename ArgType>
            void getNext(ArgType& arg)
            {
                arg = m_content.content[currentIndex].get<ArgType>();
                currentIndex++;
            }

            bool hasNextElement()
            {
                return currentIndex < m_content.content.size();
            }

            size_t argumentsCount()
            {
                return  m_content.content.size();
            }

        private:
            const OLinkContent& m_content;
            size_t currentIndex = 0;
        };

    } //Arguments

} //NlohmannSerializer





}} //namespace ApiGear::ObjectLink