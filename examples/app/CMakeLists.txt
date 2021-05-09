
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick WebSockets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick WebSockets REQUIRED)

set(APP_SOURCES
        main.cpp
        qolink.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(app
        ${APP_SOURCES}
    )
else()
    if(ANDROID)
        add_library(app SHARED
            ${APP_SOURCES}
        )
    else()
        add_executable(app
          ${APP_SOURCES}
        )
    endif()
endif()

target_compile_definitions(app
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(app
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::WebSockets)


target_link_libraries(app PRIVATE nlohmann_json::nlohmann_json olink_core)
