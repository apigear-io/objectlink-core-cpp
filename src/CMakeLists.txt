set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  # pull nlohmann json as dependency
  include(FetchContent)
  set(JSON_Install ON)
  FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
  FetchContent_MakeAvailable(json)
endif()

set(OLINK_SOURCES
    olink/core/types.cpp
    olink/core/node.cpp
    olink/core/protocol.cpp
    olink/consolelogger.cpp
    olink/clientnode.cpp
    olink/remotenode.cpp
)

SET(OLINK_HEADERS
    olink/core/types.h
    olink/core/node.h
    olink/core/protocol.h
    olink/consolelogger.h
    olink/clientnode.h
    olink/remotenode.h
)


add_library (olink_core STATIC ${OLINK_SOURCES} ${OLINK_HEADERS})

target_include_directories (olink_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(olink_core PUBLIC nlohmann_json::nlohmann_json)
