//
// ThreadPool.cpp
//
// Copyright (c) 2017 Jiawei Feng
//

#include "ThreadPool.h"
#include <util/log.h>

#include <assert.h>
#include <functional>
#include <algorithm>
#include <exception>
#include <stdio.h>

using namespace Reuzel;

ThreadPool::ThreadPool(const string &nameArg)
  : mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    name_(nameArg),
    maxQueueSize_(0),
    running_(false)
{
}

ThreadPool::~ThreadPool()
{
    if (running_) {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads_.push_back(std::unique_ptr<Thread>(
            new Thread([&](){ this->runInThread(); } )));
        threads_[i]->start();
    }
    /*
    if (numThreads == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
    */
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }
    std::for_each(threads_.begin(), threads_.end(),
        [](std::unique_ptr<Thread> &thread) { thread->join(); });
}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    size_t size = taskQueue_.size();
    return size;
}

void ThreadPool::addTask(const Task &task)
{
    if (threads_.empty()) {
        task();
    }
    else {
        MutexLockGuard lock(mutex_);
        while (isFull()) {
            notFull_.wait();
        }
        assert(!isFull());

        taskQueue_.push_back(task);
        notEmpty_.notify();
    }
}

ThreadPool::Task ThreadPool::takeTask()
{
    MutexLockGuard lock(mutex_);
    while (taskQueue_.empty() && running_) {
        notEmpty_.wait();
    }

    Task task;
    if (!taskQueue_.empty()) {
        task = taskQueue_.front();
        taskQueue_.pop_front();

        if (maxQueueSize_ > 0) {
            notFull_.notify();
        }
    }

    return task;
}

bool ThreadPool::isFull() const
{
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && taskQueue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try {
        /*
        if (threadInitCallback_) {
            threadInitCallback_();
        }
        */
        while (running_) {
            Task task(takeTask());
            if (task) {
                task();
            }
        }
    } catch (const std::exception &e) {
        ERROR("exception caught in ThreadPool %s", name_.c_str());
        abort();
    }
}
