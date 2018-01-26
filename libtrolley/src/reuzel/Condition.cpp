//
// Condition.cpp
//
// Copyright (c) 2017 Jiawei Feng
//

#include "Condition.h"
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

// return true is timeout, false otherwise
bool Reuzel::Condition::waitForSeconds(double seconds)
{
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);

    const int64_t KNanoSecondPerSecond = 1e9;
    int64_t nanoseconds = static_cast<int64_t>(seconds * KNanoSecondPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / KNanoSecondPerSecond);
    abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % KNanoSecondPerSecond);

    MutexLock::UnassignGuard ug(mutex_);
    return ETIMEDOUT == pthread_cond_timedwait(&cond_,
        mutex_.getPthreadMutex(), &abstime);
}
