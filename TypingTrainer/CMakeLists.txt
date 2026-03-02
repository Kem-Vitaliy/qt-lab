cmake_minimum_required(VERSION 3.16)

project(TypingTrainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force UTF-8 encoding for source files (important for Windows)
if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    lessonmodel.h
    lessonmodel.cpp
    lessonloader.h
    lessonloader.cpp
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

# Copy lessons/ folder to output directory after build (only if exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/lessons")
    add_custom_command(
        TARGET TypingTrainer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/lessons"
            "$<TARGET_FILE_DIR:TypingTrainer>/lessons"
        COMMENT "Copying lessons/ folder to build directory"
    )
else()
    message(STATUS "NOTE: lessons/ folder not found. Application will look for .txt files there.")
    message(STATUS "      Create lessons/ folder in project root and add .txt files.")
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TypingTrainer)
endif()
