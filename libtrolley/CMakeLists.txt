cmake_minimum_required(VERSION 2.8)
project(libtrolley)

add_subdirectory(deps/zlog)
add_subdirectory(deps/jsoncpp)
add_subdirectory(deps/gflags)
add_subdirectory(deps/libevent)
add_subdirectory(deps/libuv)
add_subdirectory(deps/restclient)
#add_subdirectory(deps/sqlite3)
add_subdirectory(deps/xredis)

include_directories(
        ${PROJECT_BINARY_DIR}/deps/gflags/include
        ${PROJECT_SOURCE_DIR}/deps/zlog/src
        ${PROJECT_SOURCE_DIR}/deps/jsoncpp/include
        ${PROJECT_SOURCE_DIR}/deps/libevent/include
        ${PROJECT_BINARY_DIR}/deps/libevent/include
        ${PROJECT_SOURCE_DIR}/deps/xredis/src
        ${PROJECT_SOURCE_DIR}/deps/libuv/include
        ${PROJECT_SOURCE_DIR}/deps/restclient/include
#        ${PROJECT_SOURCE_DIR}/deps/sqlite3/src
        ${PROJECT_SOURCE_DIR}/src
)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/src/util/log.cpp
        ${PROJECT_SOURCE_DIR}/src/util/util.cpp
        ${PROJECT_SOURCE_DIR}/src/util/cmdflags.cpp
        ${PROJECT_SOURCE_DIR}/src/util/timer.cpp
        ${PROJECT_SOURCE_DIR}/src/server/http_corouter.cpp
        ${PROJECT_SOURCE_DIR}/src/server/http_server.cpp
        ${PROJECT_SOURCE_DIR}/src/server/http_task.cpp
        ${PROJECT_SOURCE_DIR}/src/server/thread_pool.cpp
        ${PROJECT_SOURCE_DIR}/src/server/http_handler.cpp
        ${PROJECT_SOURCE_DIR}/src/client/http_client.cpp
        ${PROJECT_SOURCE_DIR}/src/client/http_sequence_num.cpp
        ${PROJECT_SOURCE_DIR}/src/client/http_client_pool.cpp
        ${PROJECT_SOURCE_DIR}/src/client/http_service_mgr.cpp
        ${PROJECT_SOURCE_DIR}/src/client/http_forkjoin.cpp
        ${PROJECT_SOURCE_DIR}/src/reuzel/Condition.cpp
        ${PROJECT_SOURCE_DIR}/src/reuzel/CountDownLatch.cpp
        ${PROJECT_SOURCE_DIR}/src/reuzel/Thread.cpp
        ${PROJECT_SOURCE_DIR}/src/reuzel/ThreadPool.cpp
        ${PROJECT_SOURCE_DIR}/src/redis/redis_client.cpp
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

find_package(Threads)
add_library(trolley_static STATIC ${SOURCES})

target_link_libraries(trolley_static 
    ${CMAKE_THREAD_LIBS_INIT} 
)

target_link_libraries(trolley_static 
    zlog_static 
    gflags 
    jsoncpp_lib_static
    event
    uv
    restclient_static
    xredis_static
)


install(TARGETS trolley_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )


install(
        DIRECTORY src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")

