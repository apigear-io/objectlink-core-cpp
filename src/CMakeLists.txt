set(OLINK_SOURCES
    olink/core/types.cpp
    olink/core/messages.cpp
    olink/core/protocol.cpp
    olink/core/listeners.cpp
    olink/core/stdoutlogger.cpp
    olink/sinktypes.cpp
    olink/sinkregistry.cpp
    olink/sourcetypes.cpp
    olink/sourceregistry.cpp
    olink/session.cpp
)

SET(OLINK_HEADERS
    olink/core/types.h
    olink/core/messages.h
    olink/core/protocol.h
    olink/core/listeners.h
    olink/core/stdoutlogger.h
    olink/sinktypes.h
    olink/sinkregistry.h
    olink/sourcetypes.h
    olink/sourceregistry.h
    olink/session.h
)


add_library (olink_core STATIC ${OLINK_SOURCES} ${OLINK_HEADERS})

target_include_directories (olink_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(olink_core PRIVATE nlohmann_json::nlohmann_json)


add_executable(tst_olink olink/tst_olink.cpp ${OLINK_SOURCES})
target_link_libraries(tst_olink PRIVATE Catch2::Catch2 nlohmann_json::nlohmann_json)
