cmake_minimum_required(VERSION 3.16)

project(TypingTrainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    lessonmodel.h
    lessonmodel.cpp
    keyboardwidget.h
    keyboardwidget.cpp
    textdisplaywidget.h
    textdisplaywidget.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TypingTrainer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(TypingTrainer
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(TypingTrainer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(TypingTrainer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TypingTrainer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TypingTrainer)
endif()
