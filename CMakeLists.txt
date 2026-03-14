cmake_minimum_required(VERSION 3.16)

project(CustomClient VERSION 0.1 LANGUAGES CXX)

# --- Options ---
option(ENABLE_CONSOLE_OUTPUT "Enable console output" ON)
option(BUILD_USER_VERSION "Build simplified user version" OFF)

# --- Standard Setup ---
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets Network Mqtt WebEngineWidgets WebChannel)
find_package(Qt6Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)

# --- FFmpeg Configuration ---
pkg_check_modules(FFMPEG REQUIRED libavcodec libavutil libswscale libavformat)

qt_standard_project_setup()

# --- Protobuf ---
# Generate Qt Protobuf classes
# Changed output directory to build directory to avoid polluting source tree
qt_add_protobuf(client_proto_lib
    PROTO_FILES rm_protocol.proto
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# --- Sources ---
set(CORE_SOURCES
    main.cpp
    rm_client.cpp rm_client.h
    types.h
    log_manager.cpp log_manager.h
    log_bridge.cpp log_bridge.h
)

set(NETWORK_SOURCES
    network/rm_sender.cpp network/rm_sender.h
    network/rm_receiver.cpp network/rm_receiver.h
)

if(BUILD_USER_VERSION)
    set(UI_SOURCES
        ui/user/mainwindow.cpp ui/user/mainwindow.h
        ui/user/web_bridge.cpp ui/user/web_bridge.h
        ui/user/user.qrc
    )
else()
    set(UI_SOURCES
        ui/full/mainwindow.cpp ui/full/mainwindow.h
        ui/full/dashboard_widget.cpp ui/full/dashboard_widget.h
        ui/full/robot_status_widget.cpp ui/full/robot_status_widget.h
        ui/full/map_widget.cpp ui/full/map_widget.h
        ui/full/command_console_widget.cpp ui/full/command_console_widget.h
        ui/full/connection_status_widget.cpp ui/full/connection_status_widget.h
    )
endif()

set(VIDEO_SOURCES
    video/video_decoder.cpp video/video_decoder.h
    video/video_widget.cpp video/video_widget.h
)

# --- Executable ---
qt_add_executable(CustomClient
    ${CORE_SOURCES}
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${VIDEO_SOURCES}
)

# --- Configuration ---
if(ENABLE_CONSOLE_OUTPUT)
    target_compile_definitions(CustomClient PRIVATE ENABLE_CONSOLE_OUTPUT)
endif()

if(BUILD_USER_VERSION)
    target_compile_definitions(CustomClient PRIVATE USER_VERSION_BUILD)
endif()

if(UI_RELEASE_MODE)
    target_compile_definitions(CustomClient PRIVATE UI_RELEASE_MODE)
endif()

# Enable fast string concatenation globally
add_compile_definitions(QT_USE_QSTRINGBUILDER)

# --- Linking ---
target_link_libraries(CustomClient PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Mqtt
    Qt6::Protobuf
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    client_proto_lib
    ${FFMPEG_LIBRARIES}
)

target_include_directories(CustomClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FFMPEG_INCLUDE_DIRS}
)

if(BUILD_USER_VERSION)
    target_include_directories(CustomClient PRIVATE ui/user)
else()
    target_include_directories(CustomClient PRIVATE ui/full)
endif()

# --- Precompiled Headers ---
target_precompile_headers(CustomClient PRIVATE
    <QMainWindow>
    <QApplication>
    <QWidget>
    <QString>
    <QVector>
    <QMap>
    <QDebug>
    <QtMqtt/QMqttClient>
)

# --- Build Properties ---
set_target_properties(CustomClient PROPERTIES 
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
)

# --- Unit Tests ---
enable_testing()
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(unit_tests
    tests/auto_tests.cpp
    ui/full/connection_status_widget.cpp
    ui/full/connection_status_widget.h
    log_bridge.cpp
    log_bridge.h
    log_manager.cpp
    log_manager.h
)

target_link_libraries(unit_tests PRIVATE
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME component_tests COMMAND unit_tests)
