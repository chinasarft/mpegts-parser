cmake_minimum_required(VERSION 3.9.0)

if(NOT DEFINED TOP_DIR) 
    message(FATAL_ERROR "should define TOP_DIR")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${TOP_DIR}/src/container/mpegts")
include_directories("${TOP_DIR}/third_party/spdlog")


add_executable(parsets
	parsets.c
	inctime.h
	inctime.c
)

target_link_libraries(parsets tsparser tools)
