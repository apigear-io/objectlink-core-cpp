set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

enable_testing()

find_package(Catch2 REQUIRED)
find_package(trompeloeil REQUIRED)

set(TEST_OLINK_SOURCES
    test_main.cpp
    test_olink.cpp
    test_protocol.cpp
    test_client_registry.cpp
    test_client_node.cpp
    test_remote_registry.cpp
    test_uniqueidstorage.cpp
    test_remote_node.cpp
    test_thread_safety.cpp
    sinkobject.hpp
    sourceobject.hpp
    mocks.h
    matchers.h
    )

add_executable(tst_olink ${TEST_OLINK_SOURCES})

add_test(tst_olink tst_olink)
target_link_libraries(tst_olink PRIVATE olink_core Catch2::Catch2 trompeloeil::trompeloeil)

# Defensive compiler flags for tests only — catch as much as possible early.
# These are NOT applied to the library itself to avoid enforcing them on consumers.
if(NOT MSVC)
  target_compile_options(tst_olink PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -fvisibility=hidden
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
  )
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(tst_olink PRIVATE
      -Wmisleading-indentation
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
      -Wuseless-cast
    )
  endif()
else()
  target_compile_options(tst_olink PRIVATE /W4 /WX /w14242 /w14254 /w14263 /w14265 /w14287 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906 /w14928 PUBLIC /wd4251)
endif()

# Sources for recompiled library variants (noexcept, static)
set(OLINK_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/olink/core/basenode.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/core/protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/core/types.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/clientnode.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/clientregistry.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/remotenode.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/remoteregistry.cpp
    ${CMAKE_SOURCE_DIR}/src/olink/consolelogger.cpp
)

# No-exceptions test: recompile library with -fno-exceptions to verify
# the code compiles and runs without C++ exception support (e.g. Unreal Engine).
# Only on GCC/Clang — MSVC equivalent (/EHs-c-) has STL header quirks.
if(NOT MSVC)
  add_library(olink_core_noexcept STATIC ${OLINK_CORE_SOURCES})
  target_include_directories(olink_core_noexcept PUBLIC ${CMAKE_SOURCE_DIR}/src)
  target_link_libraries(olink_core_noexcept PUBLIC nlohmann_json::nlohmann_json)
  target_compile_options(olink_core_noexcept PRIVATE -fno-exceptions)
  target_compile_definitions(olink_core_noexcept PUBLIC JSON_NOEXCEPTION=1)

  add_executable(tst_olink_noexcept test_noexcept.cpp)
  target_link_libraries(tst_olink_noexcept PRIVATE olink_core_noexcept)
  target_compile_options(tst_olink_noexcept PRIVATE -fno-exceptions)
  target_compile_definitions(tst_olink_noexcept PRIVATE JSON_NOEXCEPTION=1)
  add_test(NAME tst_olink_noexcept COMMAND tst_olink_noexcept)
endif()

# Static-build test: recompile library with OLINK_STATIC to verify the
# OLINK_EXPORT macro resolves to empty (no dllexport/dllimport), as used
# in monolithic builds (e.g. Unreal Engine Shipping configuration).
add_library(olink_core_static STATIC ${OLINK_CORE_SOURCES})
target_include_directories(olink_core_static PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(olink_core_static PUBLIC nlohmann_json::nlohmann_json)
target_compile_definitions(olink_core_static PUBLIC OLINK_STATIC=1)

add_executable(tst_olink_static test_static.cpp)
target_link_libraries(tst_olink_static PRIVATE olink_core_static)
target_compile_definitions(tst_olink_static PRIVATE OLINK_STATIC=1)
add_test(NAME tst_olink_static COMMAND tst_olink_static)

