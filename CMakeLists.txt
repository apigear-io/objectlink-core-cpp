cmake_minimum_required(VERSION 3.14)
project(objectlink-core-cpp LANGUAGES CXX)

include(CTest)
option(BUILD_EXAMPLES "Build examples" FALSE)
option(PERFORMANCE_TESTS "Build performance tests" FALSE)
option(OLINK_CORE_BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory (src)

if(OLINK_CORE_BUILD_TESTS)
    add_subdirectory (tests)
endif() # OLINK_CORE_BUILD_TESTS

if(PERFORMANCE_TESTS)
    add_subdirectory (performance)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory (examples/app)
    add_subdirectory (examples/server)
endif()
