cmake_minimum_required(VERSION 2.6.4)
project(xredis)

add_subdirectory(deps/hiredis)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/src/xRedisClient.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_connection.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_hashs.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_keys.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_lists.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_pubsub.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_sets.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_sortedsets.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClient_strings.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisClusterClient.cpp
        ${PROJECT_SOURCE_DIR}/src/xRedisPool.cpp
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


add_library(xredis_static STATIC ${SOURCES})

find_package(Threads)
target_link_libraries(xredis_static ${CMAKE_THREAD_LIBS_INIT} hiredis_static)

install(TARGETS xredis_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")