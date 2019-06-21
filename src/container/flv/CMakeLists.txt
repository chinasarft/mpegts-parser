cmake_minimum_required(VERSION 3.9.0)

project(flvparser)

if(NOT DEFINED TOP_DIR) 
    message(FATAL_ERROR "should define TOP_DIR")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${TOP_DIR}/src/tools")
include_directories("${TOP_DIR}/third_party/spdlog")

add_library(flvparser STATIC
	flv_struct.h
	flvparser.h
	flvparser.cpp
)

message("----------${CMAKE_BINARY_DIR}/lib/Debug")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        install(TARGETS flvparser
                DESTINATION "${CMAKE_BINARY_DIR}/lib/Debug"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
else()
        install(TARGETS flvparser
                DESTINATION "${CMAKE_BINARY_DIR}/lib/Release"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
endif()
