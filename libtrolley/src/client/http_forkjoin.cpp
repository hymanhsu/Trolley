#include "http_forkjoin.h"
#include <util/cmdflags.h>
#include <util/log.h>

HttpForkJoin * HttpForkJoin::m_p_instance = NULL;

static CLock instance_lock;

HttpForkJoin * HttpForkJoin::GetInstance(){
    if(NULL == m_p_instance){   //Double-Checked Locking Pattern
        CScopeLock lock(instance_lock);
        if(NULL == m_p_instance){
            m_p_instance = new HttpForkJoin();
        }
    }
    return m_p_instance;
}


/**
 * 初始化
**/
int HttpForkJoin::Init(){
    m_pool = new Reuzel::ThreadPool();
    m_pool->setMaxQueueSize(FLAGS_fjPoolQueueSize);
    m_pool->start(FLAGS_fjPoolWorkerNum);
}

/**
 * 销毁
**/
int HttpForkJoin::Destroy(){
    m_pool->stop();
    delete m_pool;
}


