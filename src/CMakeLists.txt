cmake_minimum_required(VERSION 3.13)
project(PvZHelper)

set(CMAKE_CXX_STANDARD 11)

set(-DCMAKE_PREFIX_PATH=/Applications/Qt/5.12.3/clang_64/lib/cmake)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Widgets)

add_executable(PvZHelper
        main.cpp
        mainwindow.cpp
        memory.cpp
        pvz.cpp
        code.cpp
        list.cpp
        window.cpp)

qt5_use_modules(PvZHelper Widgets)

