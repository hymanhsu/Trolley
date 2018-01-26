//
// Thread.cpp
//
// Copyright (c) 2017 Jiawei Feng
//

#include "Thread.h"
#include "CurrentThread.h"
#include <util/log.h>

#include <assert.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace Reuzel {
namespace CurrentThread {
    __thread pid_t t_cachedTid = 0;

    void cacheTid()
    {
        if (t_cachedTid == 0) {
            t_cachedTid = ::syscall(SYS_gettid);
        }
    }

    pid_t gettid()
    {
        if (t_cachedTid == 0) {
            cacheTid();
        }
        return t_cachedTid;
    }
}
}

using namespace Reuzel;
static void *startThread(void *arg)
{
    Thread *thread = static_cast<Thread *>(arg);
    thread->runInThread();
    return NULL;
}

Thread::Thread(const ThreadFunc &func)
  : started_(false),
    joined_(false),
    pthreadId_(0)
{
    func_ = func;
}

Thread::~Thread()
{
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;

    if (pthread_create(&pthreadId_, NULL, startThread, this) != 0) {
        started_ = false;
        ERROR("Faileed in pthread_create");
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}

void Thread::runInThread()
{
    try {
        func_();
    }
    catch (const std::exception &e) {
        ERROR("exception caught in Thread %ld", pthreadId_);
        abort();
    }
}
