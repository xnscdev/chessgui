cmake_minimum_required(VERSION 3.5)

project(chessgui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
include(GNUInstallDirs)

add_compile_definitions(CONF_DIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}/chessgui")
add_compile_definitions(DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/chessgui")

set(PROJECT_SOURCES
        main.cpp
        chessgui.cpp chessgui.h chessgui.ui
        promotiondialog.cpp promotiondialog.h promotiondialog.ui
        aspectratiowidget.cpp aspectratiowidget.h
        boardwidget.cpp boardwidget.h
        piece.cpp piece.h
        gamevariant.cpp gamevariant.h
        assets.cpp assets.h
        position.cpp position.h
        moveslistwidget.cpp moveslistwidget.h
        uciengine.cpp uciengine.h
        moveinputmethod.cpp moveinputmethod.h)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chessgui
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    if (ANDROID)
        add_library(chessgui SHARED
                ${PROJECT_SOURCES}
                )
    else ()
        add_executable(chessgui
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

target_link_libraries(chessgui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(chessgui PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.github.xnscdev.chessgui
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chessgui)
endif ()

install(TARGETS chessgui RUNTIME)
