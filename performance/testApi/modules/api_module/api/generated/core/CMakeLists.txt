set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  # pull nlohmann json as dependency
  message(STATUS "nlohmann_json NOT FOUND, fetching the release package")
  include(FetchContent)
  set(JSON_Install ON)
  FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.7.3)
  FetchContent_MakeAvailable(json)
endif()

set (SOURCES_CORE_SUPPORT
    api.json.adapter.cpp
    testapi.publisher.cpp
    testapi.threadsafedecorator.cpp
)
add_library(api-core SHARED ${SOURCES_CORE_SUPPORT})
add_library(api::api-core ALIAS api-core)
target_include_directories(api-core
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../modules>
    $<INSTALL_INTERFACE:include/api>
)
target_link_libraries(api-core PUBLIC api::api-api nlohmann_json::nlohmann_json)
# ensure maximum compiler support
if(NOT MSVC)
  target_compile_options(api-core PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
else()
  target_compile_options(api-core PRIVATE /W4 /WX /wd4251)
endif()

install(TARGETS api-core
        EXPORT ApiCoreTargets
        RUNTIME DESTINATION bin                 COMPONENT Runtime
        LIBRARY DESTINATION lib                 COMPONENT Runtime
        ARCHIVE DESTINATION lib/api   COMPONENT Development)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/api/generated FILES_MATCHING PATTERN "*.h")

export(EXPORT ApiCoreTargets
  NAMESPACE api::
)

install(EXPORT ApiCoreTargets
  FILE ApiCoreTargets.cmake
  DESTINATION ${InstallDir}
  NAMESPACE api::
)
