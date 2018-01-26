//
// Thread.h
//
// Copyright (c) 2017 Jiawei Feng
//

#ifndef THREAD_H
#define THREAD_H

#include <reuzel/CurrentThread.h>

#include <pthread.h>
#include <functional>
#include <assert.h>
#include <exception>


namespace Reuzel {
    class Thread {
    public:
        typedef std::function<void ()> ThreadFunc;

        explicit Thread(const ThreadFunc& func);
        ~Thread();

        Thread(const Thread&) = delete;
        Thread &operator=(const Thread&) = delete;

        void start();
        int join();

        bool started() const
        {
            return started_;
        }

        pthread_t pthreadId() const
        {
            return pthreadId_;
        }

        pid_t threadId() const
        {
            return CurrentThread::gettid();
        }

        void runInThread();

    private:
        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        ThreadFunc func_;
    };
}

#endif
