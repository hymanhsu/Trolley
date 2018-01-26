//
// Condition.h
//
// Copyright (c) 2017 Jiawei Feng
//

#ifndef CONDITION_H
#define CONDITION_H

#include <reuzel/Mutex.h>
#include <pthread.h>

namespace Reuzel {
    class Condition {
    public:
        explicit Condition(MutexLock &mutex)
          : mutex_(mutex)
        {
            MCHECK(pthread_cond_init(&cond_, NULL));
        }

        ~Condition()
        {
            MCHECK(pthread_cond_destroy(&cond_));
        }

        Condition(const Condition &) = delete;
        Condition &operator=(const Condition&) = delete;

        void wait()
        {
            MutexLock::UnassignGuard ug(mutex_);
            MCHECK(pthread_cond_wait(&cond_, mutex_.getPthreadMutex()));
        }

        // return true if timeout, false otherwise
        bool waitForSeconds(double seconds);

        void notify()
        {
            MCHECK(pthread_cond_signal(&cond_));
        }

        void notifyAll()
        {
            MCHECK(pthread_cond_broadcast(&cond_));
        }

    private:
        MutexLock &mutex_;
        pthread_cond_t cond_;
    };
}

#endif
