cmake_minimum_required(VERSION 2.6.4)
project(zlog)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/src/buf.c
        ${PROJECT_SOURCE_DIR}/src/category.c
        ${PROJECT_SOURCE_DIR}/src/category_table.c
        ${PROJECT_SOURCE_DIR}/src/conf.c
        ${PROJECT_SOURCE_DIR}/src/event.c
        ${PROJECT_SOURCE_DIR}/src/format.c
        ${PROJECT_SOURCE_DIR}/src/level.c
        ${PROJECT_SOURCE_DIR}/src/level_list.c
        ${PROJECT_SOURCE_DIR}/src/mdc.c
        ${PROJECT_SOURCE_DIR}/src/record.c
        ${PROJECT_SOURCE_DIR}/src/record_table.c
        ${PROJECT_SOURCE_DIR}/src/rotater.c
        ${PROJECT_SOURCE_DIR}/src/rule.c
        ${PROJECT_SOURCE_DIR}/src/spec.c
        ${PROJECT_SOURCE_DIR}/src/thread.c
        ${PROJECT_SOURCE_DIR}/src/zc_arraylist.c
        ${PROJECT_SOURCE_DIR}/src/zc_hashtable.c
        ${PROJECT_SOURCE_DIR}/src/zc_profile.c
        ${PROJECT_SOURCE_DIR}/src/zc_util.c
        ${PROJECT_SOURCE_DIR}/src/zlog-chk-conf.c
        ${PROJECT_SOURCE_DIR}/src/zlog.c
        )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g --std=c99 -pedantic -Wall -Wextra -Wno-unused-parameter")
add_definitions(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64)
if (APPLE)
    add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_BUILE_TYPE DEBUG)
    set(CMAKE_C_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
else()
    add_definitions(-D_GNU_SOURCE -D_POSIX_C_SOURCE=200112)
    add_definitions(-Wstrict-aliasing)
    add_definitions(-std=c++11)
    add_definitions(-fPIC)
    add_definitions(-g)
endif()


add_library(zlog_static STATIC ${SOURCES})
find_library(M_LIB NAMES m)
find_package(Threads)
target_link_libraries(zlog_static ${CMAKE_THREAD_LIBS_INIT} ${M_LIB})

install(TARGETS zlog_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")