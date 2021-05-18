#pragma once

#include "olink/core/types.h"

namespace ApiGear { namespace ObjectLink {

class BaseRegistry {
public:
    BaseRegistry(std::string name);
    virtual ~BaseRegistry();
    void onLog(WriteLogFunc func);
    void emitLog(LogLevel level, std::string msg);
    std::string name() const;
private:
    std::string m_nodeName;
    WriteLogFunc m_logFunc;
};

} } // ApiGear::ObjectLink
