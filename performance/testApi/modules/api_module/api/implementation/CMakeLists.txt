set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (SOURCES_CORE_IMPL
    testapi.cpp
)
add_library(api-implementation SHARED ${SOURCES_CORE_IMPL})
add_library(api::api-implementation ALIAS api-implementation)
target_include_directories(api-implementation
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../modules>
    $<INSTALL_INTERFACE:include/api>
)
target_link_libraries(api-implementation PUBLIC api::api-api api::api-core Threads::Threads)
# ensure maximum compiler support
if(NOT MSVC)
  target_compile_options(api-implementation PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
else()
  target_compile_options(api-implementation PRIVATE /W4 /WX /wd4251)
endif()

install(TARGETS api-implementation
        EXPORT ApiImplementationTargets
        RUNTIME DESTINATION bin                 COMPONENT Runtime
        LIBRARY DESTINATION lib                 COMPONENT Runtime
        ARCHIVE DESTINATION lib/api   COMPONENT Development)
# install includes
FILE(GLOB Api_INCLUDES *.h)
install(FILES ${Api_INCLUDES}
        DESTINATION include/api)

export(EXPORT ApiImplementationTargets
  NAMESPACE api::
)

install(EXPORT ApiImplementationTargets
  FILE ApiImplementationTargets.cmake
  DESTINATION ${InstallDir}
  NAMESPACE api::
)
