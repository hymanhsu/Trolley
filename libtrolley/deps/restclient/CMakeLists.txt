cmake_minimum_required(VERSION 2.6.4)
project(restclient-cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/source/restclient.cc
        ${PROJECT_SOURCE_DIR}/source/helpers.cc
        ${PROJECT_SOURCE_DIR}/source/connection.cc
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


add_library(restclient_static STATIC ${SOURCES})

find_package(Threads)

find_path(CURL_HEADER NAMES curl/curl.h)
find_library(CURL_LIB NAMES curl PATHS $ENV{CURL_LIB_PATH})

include_directories(
        ${CURL_HEADER} 
)

target_link_libraries(restclient_static 
        ${CMAKE_THREAD_LIBS_INIT} 
        ${CURL_LIB}
)

install(TARGETS restclient_static
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")