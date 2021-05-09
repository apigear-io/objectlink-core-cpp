cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(objectlink-core-cpp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory (external)
add_subdirectory (src)
#add_subdirectory (examples/app)
