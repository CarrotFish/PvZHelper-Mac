cmake_minimum_required(VERSION 3.13)
project(PvZHelper)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
IF(CMAKE_BUILD_TYPE STREQUAL Release)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks")
ENDIF()
add_subdirectory(src)
