cmake_minimum_required(VERSION 3.1)
project(performace_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(NOT TARGET check)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
endif()

add_subdirectory(testApi/modules/api_module/api)
add_subdirectory(testApi/apigear)


add_executable(test_client_side test_client_node.cpp)
add_executable(test_remote_side test_remote_node.cpp)


target_link_libraries(test_client_side olink_core api::api-olink )
target_link_libraries(test_remote_side olink_core api::api-olink api::api-implementation)
