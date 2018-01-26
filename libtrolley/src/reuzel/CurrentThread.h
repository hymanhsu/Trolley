//
// CurrentThread.h
//
// Copyright (c) 2017 Jiawei Feng
//

#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <pthread.h>

namespace Reuzel {
namespace CurrentThread {
    extern __thread pid_t t_cachedTid;

    void cacheTid();
    pid_t gettid();
}
}

#endif
