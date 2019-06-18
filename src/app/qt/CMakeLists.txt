cmake_minimum_required(VERSION 2.8.12)

set(DISSECTOR avdissector)

if (QTDIR OR DEFINED ENV{QTDIR} OR DEFINED ENV{QTDIR32} OR DEFINED ENV{QTDIR64})
	# Qt path set by user or env var
	list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
else()
	message(WARNING "QTDIR variable is missing.  Please set this variable")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QTDIR}/ib/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QTDIR}/ib/cmake/Qt5Widgets")

message("===============${CMAKE_MODULE_PATH}")

find_package(Qt5Widgets REQUIRED)

if(NOT Qt5Widgets_FOUND)
	if (ENABLE_UI)
		message(FATAL_ERROR "Failed to find Qt5")
	else()
		message(STATUS "Qt5 not found - UI disabled")
		return()
	endif()
endif()


include_directories("${TOP_DIR}/src/container")

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_AUTOMOC TRUE)

set(header_files
	mainwindow.h
)

set(source_files
	mainwindow.cpp
	main.cpp
)

set(qt_UI
	mainwindow.ui
)
qt5_wrap_ui(qt_UI_HEADERS ${qt_UI})

#set(qt_QRC dbt2.qrc)
#qt5_add_resources(qt_QRC_SOURCES ${qt_QRC})

if(MSVC)
        set(CMAKE_C_FLAGS_DEBUG "/D_CRT_SECURE_NO_WARNINGS /DDEBUG=1 /D_DEBUG=1 ${CMAKE_C_FLAGS_DEBUG}")
        set(CMAKE_CXX_FLAGS_DEBUG "/D_CRT_SECURE_NO_WARNINGS /DDEBUG=1 /D_DEBUG=1 ${CMAKE_C_FLAGS_DEBUG}")

        if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
                set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
                set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
        endif()
endif()

	

if(WIN32)
	ADD_EXECUTABLE(${DISSECTOR} WIN32 ${source_files} ${header_files} ${qt_UI_HEADERS} ${qt_QRC_SOURCES} )
	#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT dbt)
else()
	ADD_EXECUTABLE(${DISSECTOR} ${source_files} ${header_files} ${qt_UI_HEADERS} ${qt_QRC_SOURCES} )
endif()
target_link_libraries(${DISSECTOR}
	Qt5::Widgets
	tsparser
	flvparser
	tools
)
