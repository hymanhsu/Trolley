#include "thread_pool.h"
#include <util/log.h>

#include <stdlib.h>
#include <sys/sysinfo.h>
#include <limits>


ThreadNotify::ThreadNotify()
{
	pthread_mutexattr_init(&m_mutexattr);
	pthread_mutexattr_settype(&m_mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_mutex, &m_mutexattr);
    
	pthread_cond_init(&m_cond, NULL);
}

ThreadNotify::~ThreadNotify()
{
	pthread_mutexattr_destroy(&m_mutexattr);
	pthread_mutex_destroy(&m_mutex);
    
	pthread_cond_destroy(&m_cond);
}

WorkerThread::WorkerThread()
{
	m_task_cnt = 0;
    m_parent_thread_pool = NULL;
}

WorkerThread::~WorkerThread()
{
    m_running = false;
	m_thread_notify.Lock();
	m_thread_notify.Signal();
	m_thread_notify.Unlock();
}

void* WorkerThread::StartRoutine(void* arg)
{
    WorkerThread* pThread = (WorkerThread*)arg;
	pThread->Execute();
	return NULL;
}

void WorkerThread::Start()
{
	(void)pthread_create(&m_thread_id, NULL, StartRoutine, this);
}

void WorkerThread::Execute()
{
    INFO("Worker begin to run!");
    m_running = true;
	while (m_running) {
		m_thread_notify.Lock();

		// put wait in while cause there can be spurious wake up (due to signal/ENITR)
		while (m_task_list.empty()) {
			m_thread_notify.Wait();
		}

        if(!m_running){
            INFO("Worker receive stop command and exit!");
            break;
        }else{
            CommonTask* pTask = m_task_list.front();
            m_task_list.pop_front();
            --m_task_cnt;
            m_thread_notify.Unlock();

            pTask->run();

            // 如果当前线程是最忙的工作线程&&超过可以再分配的水位线&&超过任务池的平均负载水平，可以进行资源再分配
            uint32_t average_load_cnt = m_parent_thread_pool->average_load();
            if(m_thread_idx == m_parent_thread_pool->busy_thread_idx() 
                && m_task_cnt > m_parent_thread_pool->rearrange_task_waterline() 
                && m_task_cnt > average_load_cnt ) {
                uint32_t pop_task_cnt = m_task_cnt - average_load_cnt;
                CommonTask* pTempTask = NULL;
                for(int i=0; i< pop_task_cnt; i++) {
                    m_thread_notify.Lock();
                    pTempTask = m_task_list.front();
                    m_task_list.pop_front();
                    --m_task_cnt;
                    m_thread_notify.Unlock();
                    m_parent_thread_pool->AddTask(pTempTask);
                }
            }

            delete pTask;
        }

	}
}

void WorkerThread::PushTask(CommonTask* pTask)
{
	m_thread_notify.Lock();
	m_task_list.push_back(pTask);
    ++m_task_cnt;
	m_thread_notify.Signal();
	m_thread_notify.Unlock();
}

uint32_t WorkerThread::task_cnt() const
{
    return m_task_cnt;
}

void WorkerThread::set_parent_thread_pool(ThreadPool *parent_thread_pool)
{
    m_parent_thread_pool = parent_thread_pool;
}

ThreadPool::ThreadPool()
{
    m_worker_size = 0;
    m_worker_list = NULL;
}

ThreadPool::~ThreadPool()
{
}

int ThreadPool::Init(uint32_t worker_size, uint32_t rearrange_task_waterline)
{
    if(worker_size == 0) {  //如果设置线程数量为0，则取CPU数量的2倍作为线程数量
        worker_size = get_nprocs();
        worker_size *= 2;
        DEBUG("worker_size %d",worker_size);
    }

    m_worker_size = worker_size;
    m_rearrange_task_waterline = rearrange_task_waterline;
    m_worker_list = new WorkerThread [m_worker_size];
	if (!m_worker_list) {
		return -1;
	}

	for (uint32_t i = 0; i < m_worker_size; i++) {
        m_worker_list[i].set_thread_idx(i);
        m_worker_list[i].set_parent_thread_pool(this);
		m_worker_list[i].Start();
	}

	return 0;
}

void ThreadPool::Destory()
{
    if(m_worker_list)
        delete [] m_worker_list;
}

void ThreadPool::AddTask(CommonTask* pTask)
{
    m_worker_list[idle_thread_idx()].PushTask(pTask);
}

uint32_t ThreadPool::idle_thread_idx() const
{
    uint32_t _idle_thread_idx = 0;
    uint32_t min_task_cnt = std::numeric_limits<uint32_t>::max();
    uint32_t tmp_cnt;
    for(uint32_t i=0; i<m_worker_size; i++) {
        tmp_cnt = m_worker_list[i].task_cnt();
        if(min_task_cnt > tmp_cnt) {
            min_task_cnt = tmp_cnt;
            _idle_thread_idx = i;
        }
    }
    return _idle_thread_idx;
}

uint32_t ThreadPool::busy_thread_idx() const
{
    uint32_t _busy_thread_idx = 0;
    uint32_t max_task_cnt = 0;
    uint32_t tmp_cnt;
    for(uint32_t i=0; i<m_worker_size; i++) {
        tmp_cnt = m_worker_list[i].task_cnt();
        if(max_task_cnt < tmp_cnt) {
            max_task_cnt = tmp_cnt;
            _busy_thread_idx = i;
        }
    }
    return _busy_thread_idx;
}

uint32_t ThreadPool::average_load() const
{
    uint32_t total_task_cnt = 0;
    uint32_t tmp_cnt;
    for(uint32_t i=0; i<m_worker_size; i++) {
        tmp_cnt = m_worker_list[i].task_cnt();
        total_task_cnt += tmp_cnt;
    }
    return total_task_cnt/m_worker_size;
}

uint32_t ThreadPool::rearrange_task_waterline() const
{
    return m_rearrange_task_waterline;
}


