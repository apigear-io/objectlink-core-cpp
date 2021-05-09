set(OLINK_SOURCES
        olink/types.cpp
        olink/messages.cpp
        olink/protocol.cpp
        olink/listeners.cpp
        olink/client.cpp
        olink/service.cpp
        olink/stdoutlogger.cpp
)


add_library (olink_core STATIC ${OLINK_SOURCES})

target_include_directories (olink_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(olink_core PRIVATE nlohmann_json::nlohmann_json)


add_executable(tst_olink olink/tst_olink.cpp ${OLINK_SOURCES})
target_link_libraries(tst_olink PRIVATE Catch2::Catch2 nlohmann_json::nlohmann_json)
