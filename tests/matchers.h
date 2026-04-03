#pragma once

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include "olink/core/types.h"


// Parameter matcher to match a string argument
// @param keywords. List of words that has to be found in the string argument.
// @return true if all of the words from keywords were found in matched argument from function call,
//         false if one or more of keywords were missing in an argument.
inline auto contains_keywords(std::vector<std::string> keywords)
{
    return trompeloeil::make_matcher<const std::string&>(
        [](const std::string& value, const std::vector<std::string>& kw)
        {
            for (const auto& keyword : kw)
            {
                if (value.find(keyword) == std::string::npos)
                    return false;
            }
            return true;
        },

        // print lambda for error message
            [](std::ostream& os, const std::vector<std::string>& kw)
        {
            os << " matching contains_keywords( ";
            for (const auto& keyword : kw)
            {
                os << keyword << " ";
            }
            os << ")" << std::endl;
        },

            // stored keywords
            keywords
            );
}

// Parameter matcher, to match a string message in network format.
// @param keywords. List of words that has to be found in the string argument.
// @param message converter used for translation of messages.
// @return true if all of the words from keywords were found in matched argument from function call,
//         false if one or more of keywords were missing in an argument.
inline auto network_message_contains_keywords(std::vector<std::string> keywords, ApiGear::ObjectLink::MessageConverter& converter)
{
    return trompeloeil::make_matcher<const std::string&>(
        [&converter](const std::string& networkMessage, const std::vector<std::string>& kw)
        {
            auto translatedMessage = converter.fromString(networkMessage).dump();
            for (const auto& keyword : kw)
            {
                if (translatedMessage.find(keyword) == std::string::npos)
                    return false;
            }
            return true;
        },

        // print lambda for error message
            [](std::ostream& os, const std::vector<std::string>& kw)
        {
            os << " matching contains_keywords( ";
            for (const auto& keyword : kw)
            {
                os << keyword << " ";
            }
            os << ")" << std::endl;
        },

            // stored keywords
            keywords
            );
}