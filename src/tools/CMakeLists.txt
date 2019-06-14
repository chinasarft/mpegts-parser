cmake_minimum_required(VERSION 3.9.0)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_library(tools STATIC
	bitreader.h
	bitreader.c
)
