cmake_minimum_required(VERSION 3.9.0)

if(NOT DEFINED TOP_DIR) 
    message(FATAL_ERROR "should define TOP_DIR")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${TOP_DIR}/src/container")
include_directories("${TOP_DIR}/third_party/spdlog")


add_executable(avparser
	avparser.cpp
	inctime.h
	inctime.c
)

target_link_libraries(avparser tsparser flvparser tools)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	install(TARGETS avparser
		DESTINATION "${CMAKE_BINARY_DIR}/lib/Debug"
        	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
else()
	install(TARGETS avparser
		DESTINATION "${CMAKE_BINARY_DIR}/lib/Release"
        	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
endif()
add_subdirectory(qt)
