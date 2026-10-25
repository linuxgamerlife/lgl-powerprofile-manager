cmake_minimum_required(VERSION 3.20)
project(lgl-powerprofile-manager VERSION 2.0.0 LANGUAGES C CXX)

include(CTest)
include(GNUInstallDirs)
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

option(PPP_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0>=4.20)
pkg_check_modules(ADWAITA REQUIRED IMPORTED_TARGET libadwaita-1>=1.6)
pkg_check_modules(JSON_GLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)
find_program(GLIB_COMPILE_RESOURCES glib-compile-resources REQUIRED)

set(RESOURCE_XML "${CMAKE_CURRENT_SOURCE_DIR}/data/ppp.gresource.xml")
set(RESOURCE_C "${CMAKE_CURRENT_BINARY_DIR}/ppp-resources.c")
add_custom_command(
    OUTPUT "${RESOURCE_C}"
    COMMAND "${GLIB_COMPILE_RESOURCES}" "${RESOURCE_XML}"
            --generate-source --target=${RESOURCE_C}
            --sourcedir=${CMAKE_CURRENT_SOURCE_DIR}/data
    DEPENDS "${RESOURCE_XML}" data/style.css data/lgl-powerprofile-manager.png HELP.md
    VERBATIM
)
set_source_files_properties("${RESOURCE_C}" PROPERTIES COMPILE_OPTIONS "-w")

add_library(ppp-core STATIC
    src/core/profile_data.cpp
    src/core/settings.cpp
    src/core/theme.cpp
    src/core/tuned.cpp
    src/core/validation.cpp
)
target_include_directories(ppp-core PUBLIC src)
target_link_libraries(ppp-core PUBLIC PkgConfig::GLIB)
target_compile_options(ppp-core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)

add_executable(lgl-powerprofile-manager
    src/main.cpp
    src/ui/command_runner.cpp
    src/ui/helpwindow.cpp
    src/ui/status_notifier.cpp
    src/ui/themewatcher.cpp
    src/ui/window.cpp
    "${RESOURCE_C}"
)
target_include_directories(lgl-powerprofile-manager PRIVATE src)
target_compile_definitions(lgl-powerprofile-manager PRIVATE PPP_VERSION="${PROJECT_VERSION}")
target_link_libraries(lgl-powerprofile-manager PRIVATE
    ppp-core
    PkgConfig::GTKMM
    PkgConfig::ADWAITA
    PkgConfig::JSON_GLIB
)
target_compile_options(lgl-powerprofile-manager PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>>
)

if(PPP_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target ppp-core lgl-powerprofile-manager)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endforeach()
endif()

if(BUILD_TESTING)
    set(PPP_TEST_WARNING_FLAGS "")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 20)
        # GLib's g_assert macros use __COUNTER__, which Clang 20+ reports as a C2y extension.
        set(PPP_TEST_WARNING_FLAGS -Wno-c2y-extensions)
    endif()
    foreach(module profile_data tuned settings theme validation)
        add_executable(${module}_test tests/${module}_test.cpp)
        target_link_libraries(${module}_test PRIVATE ppp-core PkgConfig::GLIB)
        target_compile_options(${module}_test PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
            ${PPP_TEST_WARNING_FLAGS}
        )
        if(PPP_ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${module}_test PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${module}_test PRIVATE -fsanitize=address,undefined)
        endif()
        add_test(NAME ${module}_test COMMAND ${module}_test)
    endforeach()
endif()

install(TARGETS lgl-powerprofile-manager RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES packaging/com.linuxgamerlife.lgl-powerprofile-manager.desktop
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
install(FILES packaging/com.linuxgamerlife.lgl-powerprofile-manager.metainfo.xml
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo")
foreach(size 48 64 128 256)
    install(FILES packaging/icons/${size}x${size}/lgl-powerprofile-manager.png
            DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/${size}x${size}/apps")
endforeach()
