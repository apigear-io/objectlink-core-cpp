set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(api-api INTERFACE)
add_library(api::api-api ALIAS api-api)
target_include_directories(api-api
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../modules>
    $<INSTALL_INTERFACE:include/api>
)

# install binary files
install(TARGETS api-api
        EXPORT ApiApiTargets
        RUNTIME DESTINATION bin                 COMPONENT Runtime
        LIBRARY DESTINATION lib                 COMPONENT Runtime
        ARCHIVE DESTINATION lib/api   COMPONENT Development)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/api/generated FILES_MATCHING PATTERN "*.h")

export(EXPORT ApiApiTargets
  NAMESPACE api::
)

install(EXPORT ApiApiTargets
  FILE ApiApiTargets.cmake
  DESTINATION ${InstallDir}
  NAMESPACE api::
)
