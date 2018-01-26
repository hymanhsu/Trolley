cmake_minimum_required(VERSION 2.8)
project(test)


include_directories(
        ${PROJECT_BINARY_DIR}/../libtrolley/deps/gflags/include
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/zlog/src
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/jsoncpp/include
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/libevent/include
        ${PROJECT_BINARY_DIR}/../libtrolley/deps/libevent/include
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/xredis/src
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/libuv/include
        ${PROJECT_SOURCE_DIR}/../libtrolley/deps/restclient/include
        ${PROJECT_SOURCE_DIR}/../libtrolley/src
)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -Wall -Wextra -Wno-unused-parameter")
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
    add_definitions(-O2)
endif()


set (SERVER1_SOURCES 
        ${PROJECT_SOURCE_DIR}/server1.cpp
        )

add_executable(server1 ${SERVER1_SOURCES})
target_link_libraries(server1 
    trolley_static
)


set (TEST1_SOURCES 
        ${PROJECT_SOURCE_DIR}/flags.cpp
        ${PROJECT_SOURCE_DIR}/test1.cpp
        )

add_executable(test1 ${TEST1_SOURCES})
target_link_libraries(test1 
    trolley_static
)


set (TEST2_SOURCES
        ${PROJECT_SOURCE_DIR}/test2.cpp
        )

add_executable(test2 ${TEST2_SOURCES})
target_link_libraries(test2 
    trolley_static
)

