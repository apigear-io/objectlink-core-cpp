cmake_minimum_required(VERSION 3.14)
project(objectlink-core-cpp LANGUAGES CXX)

include(CTest)
option(BUILD_EXAMPLES "Build examples" FALSE)
option(PERFORMANCE_TESTS "Build performance tests" FALSE)
option(OLINK_CORE_BUILD_TESTS "Build tests" OFF)
option(OLINK_CORE_FETCH_DEPS "Fetch and build dependencies, if not pre-installed" OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(OLINK_CORE_FETCH_DEPS)
include(FetchContent)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  # pull nlohmann json as dependency
  message(STATUS "nlohmann_json NOT FOUND, fetching the release package")
  include(FetchContent)
  set(JSON_Install ON)
  FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
  OVERRIDE_FIND_PACKAGE)
  FetchContent_MakeAvailable(nlohmann_json)
endif()

if(OLINK_CORE_BUILD_TESTS)
find_package(Catch2 QUIET)
if (NOT Catch2_Found)
  message(STATUS "Catch2 NOT FOUND, fetching the git repository")
  FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        v2.13.8
      GIT_SHALLOW    TRUE
      OVERRIDE_FIND_PACKAGE)
  FetchContent_MakeAvailable(Catch2)
endif()

find_package(trompeloeil QUIET)
if(NOT trompeloeil_Found)
  message(STATUS "trompeloeil NOT FOUND, fetching the git repository")
  FetchContent_Declare(
      trompeloeil
      GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
      GIT_TAG        v46
      OVERRIDE_FIND_PACKAGE
      )
  FetchContent_MakeAvailable(trompeloeil)
endif()
endif() # OLINK_CORE_BUILD_TESTS
endif() # OLINK_CORE_FETCH_DEPS

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
