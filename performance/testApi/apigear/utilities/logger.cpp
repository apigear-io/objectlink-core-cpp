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
#include "logger.h"
#include <iostream>



namespace ApiGear { namespace Utilities {

static WriteLogFunc logFunc = getConsoleLogFunc(LogLevel::Warning);

void emitLog(LogLevel level, const std::string& msg){
    if(logFunc) {
        logFunc(level, msg);
    }
}

void setLog(WriteLogFunc func){
    logFunc = func;
}

WriteLogFunc getConsoleLogFunc(LogLevel minimumLevel){
    return [minimumLevel](LogLevel level, const std::string& msg)
    {
        auto levelText = "[debug  ] ";

        if (level < minimumLevel){
            return;
        }

        switch (level)
        {
            case LogLevel::Debug:
                levelText = "[debug  ] ";
                std::clog<< levelText << msg << std::endl;
                break;
            case LogLevel::Error:
                levelText = "[error  ] ";
                std::cerr<< levelText << msg << std::endl;
                break;
            case LogLevel::Warning:
                levelText = "[warning] ";
                std::cerr<< levelText << msg << std::endl;
                break;
            case LogLevel::Info:
                levelText = "[info   ] ";
                std::cout<< levelText << msg << std::endl;
                break;
            default:
                break;
        }
    };
}

void logInfo(const std::string& msg){
    emitLog(LogLevel::Info, msg);
}

void logDebug(const std::string& msg){
    emitLog(LogLevel::Debug, msg);
}

void logWarning(const std::string& msg){
    emitLog(LogLevel::Warning, msg);
}

void logError(const std::string& msg){
    emitLog(LogLevel::Error, msg);
}

} } // ApiGear::Utilities

