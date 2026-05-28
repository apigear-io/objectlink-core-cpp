// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 ApiGear UG (haftungsbeschränkt)
// Copyright (c) 2022-2026 Epic Games, Inc.

#pragma once

#include "core/types.h"

#include <string>

namespace ApiGear { namespace ObjectLink {

class OLINK_EXPORT ConsoleLogger {
public:
    static void writeLog(LogLevel level, std::string msg);
    static WriteLogFunc logFunc();
};

} } // ApiGear::ObjectLink

