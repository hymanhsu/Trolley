cmake_minimum_required(VERSION 2.6.4)
project(hiredis)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/async.c
        ${PROJECT_SOURCE_DIR}/dict.c
        ${PROJECT_SOURCE_DIR}/hiredis.c
        ${PROJECT_SOURCE_DIR}/net.c
        ${PROJECT_SOURCE_DIR}/read.c
        ${PROJECT_SOURCE_DIR}/sds.c
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


add_library(hiredis_static STATIC ${SOURCES})

find_package(Threads)
target_link_libraries(hiredis_static ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS hiredis_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")