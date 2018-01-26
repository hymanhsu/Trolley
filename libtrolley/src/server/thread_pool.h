#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <server/task.h>
#include <util/lock.h>

#include <pthread.h>
#include <list>
#include <stdint.h>

/**
 * 用于信号量通知的对象 
 **/
class ThreadNotify
{
public:
    ThreadNotify();
    ~ThreadNotify();
	void Lock() { pthread_mutex_lock(&m_mutex); }
	void Unlock() { pthread_mutex_unlock(&m_mutex); }
	void Wait() { pthread_cond_wait(&m_cond, &m_mutex); }
	void Signal() { pthread_cond_signal(&m_cond); }
private:
	pthread_mutex_t 	m_mutex;
	pthread_mutexattr_t	m_mutexattr;
	pthread_cond_t 		m_cond;
};


class ThreadPool;
class WorkerThread {
public:
    WorkerThread();
    ~WorkerThread();

    /**
    * 工作线程的入口函数
    **/
    static void* StartRoutine(void* arg);

    void Start();
    void Execute();
    void PushTask(Task* pTask);

    void set_thread_idx(uint32_t idx) {
        m_thread_idx = idx; 
    }
    uint32_t task_cnt() const;
    void set_parent_thread_pool(ThreadPool *parent_thread_pool);

private:
    bool            m_running;
    uint32_t		m_thread_idx;
    uint32_t		m_task_cnt;
    pthread_t		m_thread_id;
    ThreadNotify	m_thread_notify;
    std::list<Task*>     m_task_list;
    ThreadPool      *m_parent_thread_pool;
};

class ThreadPool {
public:
    ThreadPool();
    virtual ~ThreadPool();

    int Init(uint32_t worker_size=6, uint32_t rearrange_task_waterline=10);
    void AddTask(Task* pTask);
    void Destory();
    
    uint32_t idle_thread_idx() const;
    uint32_t busy_thread_idx() const;
    uint32_t average_load() const;
    uint32_t rearrange_task_waterline() const;

private:
    CLock           m_lock;
    uint32_t        m_rearrange_task_waterline;   //重新分配任务的最低水位线
    uint32_t        m_worker_size;
    WorkerThread* 	m_worker_list;

};


#endif /* THREADPOOL_H_ */
