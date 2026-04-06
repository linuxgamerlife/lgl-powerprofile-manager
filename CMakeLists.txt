cmake_minimum_required(VERSION 3.16)
project(lgl-powerprofile-manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(lgl-powerprofile-manager
    main.cpp
    mainwindow.cpp
    mainwindow.h
    profiledata.h
    resources.qrc
)

target_link_libraries(lgl-powerprofile-manager PRIVATE Qt6::Core Qt6::Widgets)

target_compile_options(lgl-powerprofile-manager PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)
