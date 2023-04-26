set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(apigear COMPONENTS utilities)

set (SOURCES_OLINK
    testapiservice.cpp
    testapiclient.cpp
)
add_library(api-olink SHARED ${SOURCES_OLINK})
add_library(api::api-olink ALIAS api-olink)
target_include_directories(api-olink
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../modules>
    $<INSTALL_INTERFACE:include/api>
)
target_link_libraries(api-olink 
    PUBLIC
    olink_core
    apigear::utilities
    api::api-core
)

# ensure maximum compiler support
if(NOT MSVC)
  target_compile_options(api-olink PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
else()
  target_compile_options(api-olink PRIVATE /W4 /WX /wd4251)
endif()

install(TARGETS api-olink
        EXPORT ApiOLinkTargets
        RUNTIME DESTINATION bin                 COMPONENT Runtime
        LIBRARY DESTINATION lib                 COMPONENT Runtime
        ARCHIVE DESTINATION lib/api   COMPONENT Development)
# install includes
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/api/generated FILES_MATCHING PATTERN "*.h")

export(EXPORT ApiOLinkTargets
  NAMESPACE api::
)

install(EXPORT ApiOLinkTargets
  FILE ApiOLinkTargets.cmake
  DESTINATION ${InstallDir}
  NAMESPACE api::
)
