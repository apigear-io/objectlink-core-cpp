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

#include <functional>
#include <string>

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define APIGEAR_LOGGER __attribute__ ((dllexport))
#else
#define APIGEAR_LOGGER __declspec(dllexport)
#endif
#else
#if __GNUC__ >= 4
#define APIGEAR_LOGGER __attribute__ ((visibility ("default")))
#else
#define APIGEAR_LOGGER
#endif
#endif


namespace ApiGear { namespace Utilities {

/**
* Logging levels for logs across the application.
*/
enum APIGEAR_LOGGER LogLevel {
    Debug = 0,      // Useful for debugging during development
    Info = 1,       // Some event happened, usually not important
    Warning = 2,    // Important to know
    Error = 3       // Must know - something is wrong
};

/** A type of function to log*/
using WriteLogFunc = std::function<void(LogLevel level, const std::string& msg)>;

/* 
* Set any log function which implements the WriteLogFunc signature
* 
* Can be used to disable logging or to write to a file or database.
*
* @param func a functions which implements the WriteLogFunc signature
*/
void APIGEAR_LOGGER setLog(WriteLogFunc func);

/* 
* Get the default console log function
*
* @param minimumLevel report only log events which are at least as severe
* @return func returns the default console log function
*/
WriteLogFunc APIGEAR_LOGGER getConsoleLogFunc(LogLevel minimumLevel);

/*
* Use to log
*
* @param level specify the LogLevel
* @param msg content to be logged
*/
void APIGEAR_LOGGER emitLog(LogLevel level,const std::string& msg);

/*
* Use to log on LogLevel::Info
*/
void APIGEAR_LOGGER logInfo(const std::string& msg);

/*
* Use to log on LogLevel::Debug
*/
void APIGEAR_LOGGER logDebug(const std::string& msg);

/*
* Use to log on LogLevel::Warning
*/
void APIGEAR_LOGGER logWarning(const std::string& msg);

/*
* Use to log on LogLevel::Error
*/
void APIGEAR_LOGGER logError(const std::string& msg);

} } // ApiGear::Utilities

#define AG_LOG_INFO(str) ApiGear::Utilities::logInfo(str)
#define AG_LOG_DEBUG(str) ApiGear::Utilities::logDebug(str)
#define AG_LOG_WARNING(str) ApiGear::Utilities::logWarning(str)
#define AG_LOG_ERROR(str) ApiGear::Utilities::logError(str)
