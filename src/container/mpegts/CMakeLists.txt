cmake_minimum_required(VERSION 3.9.0)

project(tsparser)

if(NOT DEFINED TOP_DIR) 
    message(FATAL_ERROR "should define TOP_DIR")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${TOP_DIR}/src/tools")
include_directories("${TOP_DIR}/third_party/spdlog")

add_library(tsparser STATIC
	tsparser.h
	tsparser.c
)
