cmake_minimum_required(VERSION 2.6.4)
project(libuv)

option(LIBUV_BUILD_TESTS "builds libuv tests and benchmarks if set to ON" OFF)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

set(SOURCES
        ${PROJECT_SOURCE_DIR}/include/uv.h
        ${PROJECT_SOURCE_DIR}/include/tree.h
        ${PROJECT_SOURCE_DIR}/include/uv-errno.h
        ${PROJECT_SOURCE_DIR}/include/uv-threadpool.h
        ${PROJECT_SOURCE_DIR}/include/uv-version.h
        ${PROJECT_SOURCE_DIR}/src/fs-poll.c
        ${PROJECT_SOURCE_DIR}/src/heap-inl.h
        ${PROJECT_SOURCE_DIR}/src/inet.c
        ${PROJECT_SOURCE_DIR}/src/queue.h
        ${PROJECT_SOURCE_DIR}/src/threadpool.c
        ${PROJECT_SOURCE_DIR}/src/uv-common.c
        ${PROJECT_SOURCE_DIR}/src/uv-common.h
        ${PROJECT_SOURCE_DIR}/src/version.c
        )

if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600 -D_GNU_SOURCE -D_CRT_SECURE_NO_WARNINGS)
    set (SOURCES ${SOURCES}
            ${PROJECT_SOURCE_DIR}/include/uv-win.h
            ${PROJECT_SOURCE_DIR}/src/win/async.c
            ${PROJECT_SOURCE_DIR}/src/win/atomicops-inl.h
            ${PROJECT_SOURCE_DIR}/src/win/core.c
            ${PROJECT_SOURCE_DIR}/src/win/detect-wakeup.c
            ${PROJECT_SOURCE_DIR}/src/win/dl.c
            ${PROJECT_SOURCE_DIR}/src/win/error.c
            ${PROJECT_SOURCE_DIR}/src/win/fs.c
            ${PROJECT_SOURCE_DIR}/src/win/fs-event.c
            ${PROJECT_SOURCE_DIR}/src/win/getaddrinfo.c
            ${PROJECT_SOURCE_DIR}/src/win/getnameinfo.c
            ${PROJECT_SOURCE_DIR}/src/win/handle.c
            ${PROJECT_SOURCE_DIR}/src/win/handle-inl.h
            ${PROJECT_SOURCE_DIR}/src/win/internal.h
            ${PROJECT_SOURCE_DIR}/src/win/loop-watcher.c
            ${PROJECT_SOURCE_DIR}/src/win/pipe.c
            ${PROJECT_SOURCE_DIR}/src/win/thread.c
            ${PROJECT_SOURCE_DIR}/src/win/poll.c
            ${PROJECT_SOURCE_DIR}/src/win/process.c
            ${PROJECT_SOURCE_DIR}/src/win/process-stdio.c
            ${PROJECT_SOURCE_DIR}/src/win/req.c
            ${PROJECT_SOURCE_DIR}/src/win/req-inl.h
            ${PROJECT_SOURCE_DIR}/src/win/signal.c
            ${PROJECT_SOURCE_DIR}/src/win/stream.c
            ${PROJECT_SOURCE_DIR}/src/win/stream-inl.h
            ${PROJECT_SOURCE_DIR}/src/win/tcp.c
            ${PROJECT_SOURCE_DIR}/src/win/tty.c
            ${PROJECT_SOURCE_DIR}/src/win/timer.c
            ${PROJECT_SOURCE_DIR}/src/win/udp.c
            ${PROJECT_SOURCE_DIR}/src/win/util.c
            ${PROJECT_SOURCE_DIR}/src/win/winapi.c
            ${PROJECT_SOURCE_DIR}/src/win/winapi.h
            ${PROJECT_SOURCE_DIR}/src/win/winsock.c
            ${PROJECT_SOURCE_DIR}/src/win/winsock.h
            )

    add_library(uv ${SOURCES})
    target_link_libraries(uv advapi32 iphlpapi psapi userenv shell32 ws2_32)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g --std=gnu89 -pedantic -Wall -Wextra -Wno-unused-parameter")
    add_definitions(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64)
    include_directories(${PROJECT_SOURCE_DIR}/src/unix)
    set(SOURCES ${SOURCES}
            ${PROJECT_SOURCE_DIR}/include/uv-unix.h
            ${PROJECT_SOURCE_DIR}/include/uv-linux.h
            ${PROJECT_SOURCE_DIR}/include/uv-sunos.h
            ${PROJECT_SOURCE_DIR}/include/uv-darwin.h
            ${PROJECT_SOURCE_DIR}/include/uv-bsd.h
            ${PROJECT_SOURCE_DIR}/include/uv-aix.h
            ${PROJECT_SOURCE_DIR}/src/unix/async.c
            ${PROJECT_SOURCE_DIR}/src/unix/atomic-ops.h
            ${PROJECT_SOURCE_DIR}/src/unix/core.c
            ${PROJECT_SOURCE_DIR}/src/unix/dl.c
            ${PROJECT_SOURCE_DIR}/src/unix/fs.c
            ${PROJECT_SOURCE_DIR}/src/unix/getaddrinfo.c
            ${PROJECT_SOURCE_DIR}/src/unix/getnameinfo.c
            ${PROJECT_SOURCE_DIR}/src/unix/internal.h
            ${PROJECT_SOURCE_DIR}/src/unix/loop.c
            ${PROJECT_SOURCE_DIR}/src/unix/loop-watcher.c
            ${PROJECT_SOURCE_DIR}/src/unix/pipe.c
            ${PROJECT_SOURCE_DIR}/src/unix/poll.c
            ${PROJECT_SOURCE_DIR}/src/unix/process.c
            ${PROJECT_SOURCE_DIR}/src/unix/signal.c
            ${PROJECT_SOURCE_DIR}/src/unix/spinlock.h
            ${PROJECT_SOURCE_DIR}/src/unix/stream.c
            ${PROJECT_SOURCE_DIR}/src/unix/tcp.c
            ${PROJECT_SOURCE_DIR}/src/unix/thread.c
            ${PROJECT_SOURCE_DIR}/src/unix/timer.c
            ${PROJECT_SOURCE_DIR}/src/unix/tty.c
            ${PROJECT_SOURCE_DIR}/src/unix/udp.c
            )

    if (APPLE)
        add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
        set(SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/proctitle.c
                ${PROJECT_SOURCE_DIR}/src/unix/kqueue.c
                ${PROJECT_SOURCE_DIR}/src/unix/darwin.c
                ${PROJECT_SOURCE_DIR}/src/unix/fsevents.c
                ${PROJECT_SOURCE_DIR}/src/unix/pthread-barrier.c
                ${PROJECT_SOURCE_DIR}/src/unix/darwin-proctitle.c
                )

        #This is necessary to mute harmless warnings, as documented by https://github.com/issues/454
        set_source_files_properties(${PROJECT_SOURCE_DIR}/src/unix/stream.c PROPERTIES COMPILE_FLAGS -Wno-gnu-folding-constant)
    endif()

    if (NOT APPLE)
        add_definitions(-Wstrict-aliasing)
        add_definitions(-std=c++11)
        add_definitions(-g)
        #add_definitions(-static)
    else()
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_BUILE_TYPE DEBUG)
        set(CMAKE_C_FLAGS_DEBUG "-g")
        set(CMAKE_CXX_FLAGS_DEBUG "-g")
        #add_definitions(-static)
    endif()

    if ((${CMAKE_SYSTEM_NAME} MATCHES "Linux") OR (${CMAKE_SYSTEM_NAME} MATCHES "Android"))
        add_definitions(-D_GNU_SOURCE -D_POSIX_C_SOURCE=200112)
        set(SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/proctitle.c
                ${PROJECT_SOURCE_DIR}/src/unix/linux-core.c
                ${PROJECT_SOURCE_DIR}/src/unix/linux-inotify.c
                ${PROJECT_SOURCE_DIR}/src/unix/linux-syscalls.c
                ${PROJECT_SOURCE_DIR}/src/unix/linux-syscalls.h
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        add_definitions(-D__EXTENSIONS__ -D_XOPEN_SOURCE=500)
        set(SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/sunos.c
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "AIX")
        add_definitions(-D_ALL_SOURCE -D_XOPEN_SOURCE=500 -D_LINUX_SOURCE_COMPAT)
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/aic.c
                )
        find_library(PERFSTAT_LIB NAMES perfstat)
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFlyBSD")
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/freebsd.c
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/openbsd.c
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/netbsd.c
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFlyBSD" OR
            ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/src/unix/kqueue.c
                )
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set (SOURCES ${SOURCES}
                ${PROJECT_SOURCE_DIR}/include/android-ifaddrs.h
                ${PROJECT_SOURCE_DIR}/include/pthread-fixes.h
                #${PROJECT_SOURCE_DIR}/include/pthread-barrier.h   ## since 1.10 `fixes` renamed to `barrier`
                ${PROJECT_SOURCE_DIR}/src/unix/android-ifaddrs.c
                ${PROJECT_SOURCE_DIR}/src/unix/pthread-fixes.c
                #${PROJECT_SOURCE_DIR}/src/unix/pthread-barrier.c  ## since 1.10 `fixes` renamed to `barrier`
                )
    endif()

    add_library(uv STATIC ${SOURCES})

    find_package(Threads)
    target_link_libraries(uv ${CMAKE_THREAD_LIBS_INIT})

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_library(DL_LIB NAMES dl)
        find_library(RT_LIB NAMES rt)
        target_link_libraries(uv ${DL_LIB} ${RT_LIB})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
        find_library(KSTAT_LIB NAMES kstat)
        find_library(NSL_LIB NAMES nsl)
        find_library(SENDFILE_LIB NAMES sendfile)
        find_library(SOCKET_LIB NAMES socket)
        target_link_libraries(uv ${KSTAT_LIB} ${NSL_LIB} ${SENDFILE_LIB} ${SOCKET_LIB})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "AIX")
        find_library(PERFSTAT_LIB NAMES perfstat)
        target_link_libraries(uv ${PERFSTAT_LIB})
    endif()

    if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "DragonFlyBSD" OR
            ${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
        find_library(KVM_LIB NAMES kvm)
        target_link_libraries(uv ${KVM_LIB})
    endif()

endif()


install(TARGETS uv
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )


install(
        DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")