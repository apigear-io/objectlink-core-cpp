cmake_minimum_required(VERSION 3.20)
project(apigear-utilities)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (SOURCES
    logger.cpp
)
add_library(apigear-utilities SHARED ${SOURCES})
add_library(apigear::utilities ALIAS apigear-utilities)
target_include_directories(apigear-utilities
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
    $<INSTALL_INTERFACE:include>
)

# install binary files
install(TARGETS apigear-utilities
        EXPORT ApiGearUtilitiesTargets
        RUNTIME DESTINATION bin           COMPONENT Runtime
        LIBRARY DESTINATION lib           COMPONENT Runtime
        ARCHIVE DESTINATION lib           COMPONENT Development)
# install includes
FILE(GLOB_RECURSE APIGEAR_INCLUDES *.h)
install(FILES ${APIGEAR_INCLUDES}
        DESTINATION include/apigear/utilities)

export(EXPORT ApiGearUtilitiesTargets
  NAMESPACE apigear::
)

install(EXPORT ApiGearUtilitiesTargets
  FILE ApiGearUtilitiesTargets.cmake
  DESTINATION ${InstallDir}
  NAMESPACE apigear::
)
