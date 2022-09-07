#pragma once

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>
#include "olink/iobjectsink.h"
#include "olink/core/types.h"


class SinkObjectMock : public trompeloeil::mock_interface<ApiGear::ObjectLink::IObjectSink>
{
public:
    IMPLEMENT_MOCK0(olinkObjectName);
    IMPLEMENT_MOCK2(olinkOnSignal);
    IMPLEMENT_MOCK2(olinkOnPropertyChanged);
    IMPLEMENT_MOCK3(olinkOnInit);
    IMPLEMENT_MOCK0(olinkOnRelease);
};

// Fake interface to Mock write and log function calls.
class Output
{
public:
    virtual ~Output() = default;
    virtual void writeMessage(const std::string& msg) = 0;
    virtual void logMessage(ApiGear::ObjectLink::LogLevel level, const std::string& msg) = 0;
};

// Mock for write and log functions.
class OutputMock : public trompeloeil::mock_interface<Output>
{
public:
    IMPLEMENT_MOCK1(writeMessage);
    IMPLEMENT_MOCK2(logMessage);
};
