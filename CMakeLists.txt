cmake_minimum_required(VERSION 3.9.0)

project(avdissector)

SET(TOP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
if(WIN32)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8")
endif()

add_subdirectory(src/tools)
add_subdirectory(src/container/mpegts)
add_subdirectory(src/container/flv)
add_subdirectory(src/app)
