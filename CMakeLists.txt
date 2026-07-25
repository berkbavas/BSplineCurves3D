cmake_minimum_required(VERSION 3.25.1)

project(BSplineRenderer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External")
set(QT_IMGUI_DIR "${EXTERNAL_DIR}/qtimgui")
set(EIGEN_DIR "${EXTERNAL_DIR}/Eigen")

set(LIBS
    debug imguid optimized imgui
    debug qt_imgui_widgetsd optimized qt_imgui_widgets
)

file(GLOB_RECURSE SOURCES Source/*.cpp BSplineRenderer.qrc)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Gui Concurrent OpenGLWidgets)

add_executable(BSplineRenderer ${SOURCES})

target_include_directories(BSplineRenderer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${QT_IMGUI_DIR}/include"
    "${EIGEN_DIR}"
)

target_link_directories(BSplineRenderer PRIVATE "${QT_IMGUI_DIR}/lib")

target_link_libraries(BSplineRenderer Qt6::Core Qt6::Widgets Qt6::OpenGL Qt6::Concurrent Qt6::OpenGLWidgets ${LIBS})

add_custom_command(TARGET BSplineRenderer
    POST_BUILD COMMAND ${CMAKE_COMMAND}
    -E copy_directory
    "${CMAKE_SOURCE_DIR}/Resources/"
    "$<TARGET_FILE_DIR:BSplineRenderer>/Resources/"
)

add_custom_command(TARGET BSplineRenderer
    POST_BUILD COMMAND
    Qt6::windeployqt
    --dir "$<TARGET_FILE_DIR:BSplineRenderer>"
    "$<TARGET_FILE_DIR:BSplineRenderer>/$<TARGET_FILE_NAME:BSplineRenderer>"
)
