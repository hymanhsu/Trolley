//
// Mutex.h
//
// Copyright (c) 2017 Jiawei Feng
//

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <assert.h>
#include <reuzel/CurrentThread.h>

#define MCHECK(ret) ({ __typeof(ret) errnum = ret;    \
                       assert(errnum == 0); (void)errnum; })

namespace Reuzel {
    class MutexLock {
    public:
        MutexLock()
          : holder_(0)
        {
            MCHECK(pthread_mutex_init(&mutex_, NULL));
        }

        ~MutexLock()
        {
            assert(holder_ == 0);
            MCHECK(pthread_mutex_destroy(&mutex_));
        }

        MutexLock(const MutexLock&) = delete;
        MutexLock &operator=(const MutexLock&) = delete;

        bool isLockedByThisThread() const
        {
            return holder_ == CurrentThread::gettid();
        }

        void assertLocked() const
        {
            assert(isLockedByThisThread());
        }

        void lock()
        {
            MCHECK(pthread_mutex_lock(&mutex_));
            assignHolder();
        }

        void unlock()
        {
            unassignHolder();
            MCHECK(pthread_mutex_unlock(&mutex_));
        }

        pthread_mutex_t *getPthreadMutex()
        {
            return &mutex_;
        }

    private:
        friend class Condition;

        class UnassignGuard {
        public:
            UnassignGuard(MutexLock &owner)
              : owner_(owner)
            {
                owner_.unassignHolder();
            }

            ~UnassignGuard()
            {
                owner_.assignHolder();
            }

            UnassignGuard(const UnassignGuard&) = delete;
            UnassignGuard &operator=(const UnassignGuard &) = delete;
        private:
            MutexLock &owner_;
        };

        void assignHolder()
        {
            holder_ = CurrentThread::gettid();
        }

        void unassignHolder()
        {
            holder_ = 0;
        }

        pthread_mutex_t mutex_;
        pid_t holder_;
    };

    class MutexLockGuard {
    public:
        explicit MutexLockGuard(MutexLock &mutex)
          : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }
    private:
        MutexLock &mutex_;
    };
}

#endif
