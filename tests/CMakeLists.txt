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

