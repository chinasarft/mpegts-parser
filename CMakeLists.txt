cmake_minimum_required(VERSION 2.8)
set(CURRENT_TARGET "tsparser")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

list(APPEND SRCS_${CURRENT_TARGET} "bitreader.c")
list(APPEND SRCS_${CURRENT_TARGET} "bitreader.h")
list(APPEND SRCS_${CURRENT_TARGET} "tsparser.c")
list(APPEND SRCS_${CURRENT_TARGET} "tsparser.h")

foreach(f IN LISTS SRCS_${CURRENT_TARGET})
  get_filename_component(b ${f} NAME)
  set_source_files_properties(${f} PROPERTIES
    COMPILE_DEFINITIONS "__FILE_NAME__=\"${b}\"")
endforeach()


# build shared target
if(WITH_SHARED)
    set(LIB_TARGET_NAME ${CURRENT_TARGET}_SHARED)
    add_library(${CURRENT_TARGET}_SHARED SHARED ${SRCS_${CURRENT_TARGET}})
    set_target_properties(${CURRENT_TARGET}_SHARED PROPERTIES OUTPUT_NAME ${CURRENT_TARGET})
    target_compile_definitions(${CURRENT_TARGET}_SHARED PRIVATE "-D__PROJECT__=\"[${CURRENT_TARGET}]\"")
    install(TARGETS ${CURRENT_TARGET}_SHARED
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
else()
    set(LIB_TARGET_NAME ${CURRENT_TARGET}_STATIC)
    add_library(${CURRENT_TARGET}_STATIC STATIC ${SRCS_${CURRENT_TARGET}})
    set_target_properties(${CURRENT_TARGET}_STATIC PROPERTIES OUTPUT_NAME ${CURRENT_TARGET})
    target_compile_definitions(${CURRENT_TARGET}_STATIC PRIVATE "-D__PROJECT__=\"[${CURRENT_TARGET}]\"")
    install(TARGETS ${CURRENT_TARGET}_STATIC
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        )
endif()


add_executable(testtsparser main.c)
target_link_libraries(testtsparser "${LIB_TARGET_NAME}")

# ------------------------------------------------------------------------------
# Export head file
# ------------------------------------------------------------------------------

file(GLOB ${CURRENT_TARGET}_HEADERS
  tsparser.h
  )
install(FILES ${${CURRENT_TARGET}_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CURRENT_TARGET}
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
  )

