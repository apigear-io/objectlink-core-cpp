set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

find_package(nlohmann_json REQUIRED)

# needed to access CMAKE_INSTALL_LIBDIR
include(GNUInstallDirs)
set(InstallDir ${CMAKE_INSTALL_LIBDIR}/cmake/olink_core)

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
set_property(TARGET olink_core PROPERTY POSITION_INDEPENDENT_CODE ON)

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
        EXPORT olink_coreConfig)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/olink DESTINATION include FILES_MATCHING PATTERN "*.h")
include(CMakePackageConfigHelpers)
export(TARGETS
    olink_core
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/olink_coreConfig.cmake"
)
install(EXPORT
    olink_coreConfig
    DESTINATION ${InstallDir}
)
