cmake_minimum_required(VERSION 3.16)
project(lgl-powerprofile-manager VERSION 1.1.2 LANGUAGES CXX)

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

include(GNUInstallDirs)
install(TARGETS lgl-powerprofile-manager
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/lgl-powerprofile-manager.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES packaging/lgl-powerprofile-manager.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
install(FILES packaging/icons/48x48/lgl-powerprofile-manager.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps)
install(FILES packaging/icons/64x64/lgl-powerprofile-manager.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps)
install(FILES packaging/icons/128x128/lgl-powerprofile-manager.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps)
install(FILES packaging/icons/256x256/lgl-powerprofile-manager.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)
