cmake_minimum_required(VERSION 3.14)
project(apigear)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(apigear_COMPONENTS "")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/package)
set(INCLUDE_INSTALL_DIR include/apigear/ CACHE FILEPATH "")
set(LIB_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/lib/ CACHE FILEPATH "")
set(InstallDir ${LIB_INSTALL_DIR}/cmake/apigear)

add_subdirectory(utilities)
list(APPEND apigear_COMPONENTS "utilities")


include(CMakePackageConfigHelpers)
configure_package_config_file(ApigearConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/apigearConfig.cmake
  INSTALL_DESTINATION ${InstallDir}
  PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/apigearConfigVersion.cmake
  VERSION 0.1.0
  COMPATIBILITY SameMinorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/apigearConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/apigearConfigVersion.cmake
        DESTINATION ${InstallDir} )
