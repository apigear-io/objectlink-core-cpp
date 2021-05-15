
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick WebSockets REQUIRED)

set(SERVER_SOURCE
        main.cpp
        ../app/qolink.cpp
        calcsource.cpp
        qolinkserver.cpp
        qobjectlinkconnection.cpp
)

set(SERVER_HEADERS
        main.cpp
        ../app/qolink.h
        calcsource.h
        qolinkserver.h
        qobjectlinkconnection.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(server
        ${SERVER_SOURCE} ${SERVER_HEADERS}
    )
else()
    if(ANDROID)
        add_library(server SHARED
            ${SERVER_SOURCE}
        )
    else()
        add_executable(server
          ${SERVER_SOURCE} ${SERVER_HEADERS}
        )
    endif()
endif()

target_compile_definitions(server
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(server
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::WebSockets)


target_link_libraries(server PRIVATE nlohmann_json::nlohmann_json olink_core)
