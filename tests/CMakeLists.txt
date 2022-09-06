set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

if(BUILD_TESTING)
enable_testing()

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.8
    GIT_SHALLOW    TRUE
    FIND_PACKAGE_ARGS)

FetchContent_Declare(
    trompeloeil
    GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
    GIT_TAG        8480eda8bcda5ac85ef18866047f485d7d2392f5
    )
FetchContent_MakeAvailable(Catch2 trompeloeil)


set(TEST_OLINK_SOURCES
    test_main.cpp
    test_olink.cpp
    test_protocol.cpp
    test_client_registry.cpp
    sinkobject.hpp
    sourceobject.hpp
    mocksink.hpp
    mocksource.hpp
    mocks.h
         "test_client_node.cpp")

add_executable(tst_olink ${TEST_OLINK_SOURCES})

add_test(tst_olink tst_olink)
target_link_libraries(tst_olink PRIVATE olink_core Catch2::Catch2 trompeloeil::trompeloeil)

endif() # BUILD_TESTING
