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
add_library(olink_core::olink_core ALIAS olink_core)
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

# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/olink DESTINATION include FILES_MATCHING PATTERN "*.h")
include(CMakePackageConfigHelpers)
install(TARGETS olink_core
        EXPORT olink_coreTargets)
export(EXPORT olink_coreTargets
       NAMESPACE olink_core::)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/olink_coreConfig.cmake
    INSTALL_DESTINATION ${InstallDir}
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/olink_coreConfigVersion.cmake
  VERSION 0.2.8
  COMPATIBILITY SameMinorVersion)
install(EXPORT olink_coreTargets
        FILE olink_coreTargets.cmake
        DESTINATION ${InstallDir}
        NAMESPACE olink_core::)
install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/olink_coreConfig.cmake
      ${CMAKE_CURRENT_BINARY_DIR}/olink_coreConfigVersion.cmake
    DESTINATION
        ${InstallDir}
)
