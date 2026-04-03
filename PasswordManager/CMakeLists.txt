cmake_minimum_required(VERSION 3.16)

project(PasswordManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Sql REQUIRED)

add_executable(PasswordManager
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/record.h
    src/databasemanager.cpp
    src/databasemanager.h
    src/repository.cpp
    src/repository.h
    src/passwordmodel.cpp
    src/passwordmodel.h
    src/passwordproxymodel.cpp
    src/passwordproxymodel.h
)

target_link_libraries(PasswordManager PRIVATE Qt6::Widgets Qt6::Sql)

if(WIN32)
    set_target_properties(PasswordManager PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()
