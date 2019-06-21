cmake_minimum_required(VERSION 3.9.0)


include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_library(tools STATIC
	bitreader.h
	bitreader.cpp
	sps.h
	parsesps.cpp
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        install(TARGETS tools
                DESTINATION "${CMAKE_BINARY_DIR}/lib/Debug"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
else()
        install(TARGETS tools
                DESTINATION "${CMAKE_BINARY_DIR}/lib/Release"
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
endif()

#-------test
add_executable(testsps test_sps.cpp)
target_compile_options(testsps PRIVATE "-std=c++11")
target_include_directories(testsps 
	PRIVATE
	"${CMAKE_BINARY_DIR}/third_party/googletest/googletest/include"
	"${CMAKE_BINARY_DIR}/third_party/googletest/googlemock/include"
	)
target_link_directories(testsps PRIVATE "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
target_link_libraries(testsps gtest gmock_main tools)
add_test(NAME testcase COMMAND testsps)
