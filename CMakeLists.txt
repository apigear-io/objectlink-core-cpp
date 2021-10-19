cmake_minimum_required(VERSION 3.14)

if(DEFINED PROJECT_NAME)
    set(SUBPROJECT ON)
endif()


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(objectlink-core-cpp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory (src)
add_subdirectory (tests)


if(NOT SUBPROJECT)
    add_subdirectory (examples/app)
    add_subdirectory (examples/server)
endif()
