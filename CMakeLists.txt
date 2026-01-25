cmake_minimum_required(VERSION 3.16)

project(rm_test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix for "file too big" / "too many sections" error on MinGW with large generated files
if(MINGW)
    add_compile_options("-Wa,-mbig-obj")
endif()

# Find Qt6 components
# Made Mqtt and Protobuf REQUIRED to ensure they are available
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network Mqtt Protobuf)

qt_standard_project_setup()

# Generate Qt Protobuf classes
# Changed output directory to build directory to avoid polluting source tree
qt_add_protobuf(rm_proto_lib
    PROTO_FILES rm_protocol.proto
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

qt_add_executable(rm_test
    main.cpp
    mainwindow.cpp
    mainwindow.h
    rm_client.h
    rm_client.cpp
    types.h
    log_manager.h
    log_manager.cpp
    network/rm_sender.h
    network/rm_sender.cpp
    network/rm_receiver.h
    network/rm_receiver.cpp
    ui/dashboard_widget.h
    ui/dashboard_widget.cpp
    ui/robot_status_widget.h
    ui/robot_status_widget.cpp
    ui/map_widget.h
    ui/map_widget.cpp
    ui/command_console_widget.h
    ui/command_console_widget.cpp
    ui/connection_status_widget.h
    ui/connection_status_widget.cpp
    log_bridge.h
    log_bridge.cpp
    video/video_decoder.h
    video/video_decoder.cpp
    video/video_widget.h
    video/video_widget.cpp
)

target_link_libraries(rm_test PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::Mqtt
    Qt6::Protobuf
    rm_proto_lib
)

# Enable fast string concatenation globally
add_compile_definitions(QT_USE_QSTRINGBUILDER)

# Precompile common Qt headers
target_precompile_headers(rm_test PRIVATE
    <QMainWindow>
    <QApplication>
    <QWidget>
    <QString>
    <QVector>
    <QMap>
    <QDebug>
    <QtMqtt/QMqttClient>
)

set_target_properties(rm_test PROPERTIES 
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 16
)

# FFmpeg Configuration
if(WIN32)
    set(FFMPEG_ROOT "D:/Code/RM26/open/ffmpeg-8.0.1-full_build-shared")
    
    target_include_directories(rm_test PRIVATE 
        ${FFMPEG_ROOT}/include
    )
    
    target_link_directories(rm_test PRIVATE 
        ${FFMPEG_ROOT}/lib
    )
    
    target_link_libraries(rm_test PRIVATE 
        avformat
        avcodec
        swscale
        avutil
    )
    
    # Copy FFmpeg DLLs to the executable directory after build
    add_custom_command(TARGET rm_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${FFMPEG_ROOT}/bin
        $<TARGET_FILE_DIR:rm_test>
        COMMENT "Copying FFmpeg DLLs to output directory..."
    )
else()
    # Linux Configuration
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FFMPEG REQUIRED libavcodec libavutil libswscale libavformat)
    
    target_include_directories(rm_test PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(rm_test PRIVATE ${FFMPEG_LIBRARIES})
endif()

# Unit Tests
enable_testing()
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(unit_tests
    tests/auto_tests.cpp
    ui/connection_status_widget.cpp
    ui/connection_status_widget.h
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
