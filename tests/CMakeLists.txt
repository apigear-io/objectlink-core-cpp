set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

Include(FetchContent)

find_package(Catch2 QUIET)
if(NOT Catch2_FOUND)
  # pull catch2 as dependency
  message(STATUS "Catch2 NOT FOUND, fetching the git repository")
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.8
    GIT_SHALLOW    TRUE)
  FetchContent_MakeAvailable(Catch2)
endif()

set(TEST_OLINK_SOURCES
    test_main.cpp
    test_olink.cpp
    test_protocol.cpp
    sinkobject.hpp
    sourceobject.hpp
    mocksink.hpp
    mocksource.hpp
)

add_executable(tst_olink ${TEST_OLINK_SOURCES})
target_link_libraries(tst_olink PRIVATE olink_core Catch2::Catch2)

