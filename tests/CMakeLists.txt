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
    sinkobject.hpp
    sourceobject.hpp
    mocks.h
    matchers.h
    )

add_executable(tst_olink ${TEST_OLINK_SOURCES})

add_test(tst_olink tst_olink)
target_link_libraries(tst_olink PRIVATE olink_core Catch2::Catch2 trompeloeil::trompeloeil)

# ensure maximum compiler support
if(NOT MSVC)
  target_compile_options(tst_olink PRIVATE -Wall -Wextra -Wpedantic -Werror -fvisibility=hidden)
else()
  target_compile_options(tst_olink PRIVATE /W4 /WX PUBLIC /wd4251)
endif()

