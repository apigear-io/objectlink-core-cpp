// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#include "consolelogger.h"
#include <string>
#include <iostream>

namespace ApiGear { namespace ObjectLink {

void ConsoleLogger::writeLog(LogLevel level, std::string msg)
{
    switch(level) {
    case LogLevel::Info:
        std::cout << "[info   ] ";
        break;
    case LogLevel::Debug:
        std::cout << "[debug  ] ";
        break;
    case LogLevel::Warning:
        std::cout << "[warning] ";
        break;
    case LogLevel::Error:
        std::cout << "[error  ] ";
        break;
    }
    std::cout << msg << std::endl;
}

WriteLogFunc ConsoleLogger::logFunc()
{
    return [](LogLevel level, std::string msg) {
        ConsoleLogger::writeLog(level, msg);
    };
}

} } // ApiGear::ObjectLink


