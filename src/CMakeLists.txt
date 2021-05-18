set(SPDLOG_DEBUG_ON true)
set(SPDLOG_TRACE_ON true)

set(OLINK_SOURCES
    olink/core/types.cpp
    olink/core/protocol.cpp
    olink/core/listeners.cpp
    olink/core/consolelogger.cpp
    olink/sink/sinktypes.cpp
    olink/sink/sinkregistry.cpp
    olink/sink/sinknode.cpp
    olink/source/sourcetypes.cpp
    olink/source/sourceregistry.cpp
    olink/source/sourcenode.cpp
    olink/shared/basenode.cpp
    olink/shared/baseregistry.cpp
)

SET(OLINK_HEADERS
    olink/core/types.h
    olink/core/protocol.h
    olink/core/listeners.h
    olink/core/consolelogger.h
    olink/sink/sinktypes.cpp
    olink/sink/sinkregistry.cpp
    olink/sink/sinknode.cpp
    olink/source/sourcetypes.cpp
    olink/source/sourceregistry.cpp
    olink/source/sourcenode.cpp
    olink/shared/basenode.cpp
    olink/shared/baseregistry.cpp
)


add_library (olink_core STATIC ${OLINK_SOURCES} ${OLINK_HEADERS})

target_include_directories (olink_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(olink_core PUBLIC nlohmann_json::nlohmann_json)

set(TEST_OLINK_SOURCES
    tests/tst_olink.cpp
    tests/sinkobject.hpp
    tests/sourceobject.hpp
)

add_executable(tst_olink ${TEST_OLINK_SOURCES})
target_link_libraries(tst_olink PRIVATE olink_core Catch2::Catch2)
