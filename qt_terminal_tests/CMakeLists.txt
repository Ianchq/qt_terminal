cmake_minimum_required(VERSION 3.14)
project(TerminalWidgetTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/../build
)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../")

find_package(Qt5 REQUIRED COMPONENTS Widgets Test Gui)

add_library(terminal_widget STATIC
    ../terminaltextedit.cpp
    ../mainwindow.cpp
    ../mainwindow.h
    ../terminaltextedit.h
    ../mainwindow.ui
)

target_link_libraries(terminal_widget Qt5::Widgets Qt5::Gui Qt5::Test)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(tests)
