set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  # pull nlohmann json as dependency
  message(STATUS "nlohmann_json NOT FOUND, fetching the release package")
  include(FetchContent)
  set(JSON_Install ON)
  FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
  FetchContent_MakeAvailable(json)
endif()

set(OLINK_SOURCES
    olink/core/basenode.cpp
    olink/core/protocol.cpp
    olink/core/types.cpp
    olink/consolelogger.cpp
    olink/clientnode.cpp
    olink/clientregistry.cpp
    olink/remotenode.cpp
    olink/remoteregistry.cpp 
    )

add_library (olink_core STATIC ${OLINK_SOURCES} )

target_include_directories (olink_core 
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(olink_core PUBLIC nlohmann_json::nlohmann_json)

# ensure maximum compiler support
if(NOT MSVC)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(olink_core PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden -Wdocumentation)
else()
  target_compile_options(olink_core PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
endif()
else()
  target_compile_options(olink_core PRIVATE /W4 /WX PUBLIC /wd4251)
endif()

# install binary files
install(TARGETS olink_core
        EXPORT olink_coreConfig
        RUNTIME DESTINATION bin COMPONENT Runtime
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib/olink_core COMPONENT Development)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/olink DESTINATION include FILES_MATCHING PATTERN "*.h")
include(CMakePackageConfigHelpers)
export(TARGETS
    olink_core
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/olink_coreConfig.cmake"
)
install(EXPORT
    olink_coreConfig
    DESTINATION "lib/olink_core/cmake"
)
