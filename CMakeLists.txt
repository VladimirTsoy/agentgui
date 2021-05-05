cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_definitions(-DUNICODE -D_UNICODE)
project(AgentGUI LANGUAGES CXX VERSION 0.1.0)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB SOURCES
    ./src/*.cpp
    ./src/*.h
    ./src/*.ui
    ./src/*.qrc
)

find_package(Qt5 COMPONENTS Gui Widgets Network REQUIRED)
message(STATUS "Qt5Gui Version:  ${Qt5Gui_VERSION}")
message(STATUS "QtWidgets Version:  ${Qt5Widgets_VERSION}")
message(STATUS "Qt5Network Version:  ${Qt5Network_VERSION}")

include_directories(${CMAKE_BINARY_DIR})
configure_file("${CMAKE_SOURCE_DIR}/version.h.in" "${CMAKE_BINARY_DIR}/version.h" @ONLY)

find_package(AthenaAgent REQUIRED)
if (ATHENA_AGENT_FOUND)
    include_directories(${ATHENA_AGENT_INCLUDE_DIRS})
endif()

add_executable(AgentGUI ${SOURCES})

target_include_directories(AgentGUI PUBLIC "${PROJECT_BINARY_DIR}")

message("CMAKE_BINARY_DIR = "${CMAKE_BINARY_DIR})
if (UNIX AND NOT APPLE)
    target_compile_definitions(AgentGUI PUBLIC SHARED_APP_DATA_PATH="${CMAKE_BINARY_DIR}/../../AthenaAgent/AVSoft/AthenaAgent")
endif()

if (NOT UNIX)
    target_compile_definitions(AgentGUI PUBLIC SHARED_APP_DATA_PATH="C:/ProgramData")
endif()


target_link_libraries(AgentGUI Qt5::Gui Qt5::Widgets Qt5::Network)
