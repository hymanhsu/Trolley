#ifndef __HTTP_FORKJOIN_H__
#define __HTTP_FORKJOIN_H__


#include <reuzel/ThreadPool.h>
#include <reuzel/CountDownLatch.h>
#include <util/lock.h>

#include <restclient-cpp/restclient.h>

#include <stdint.h>
#include <unistd.h>
#include <vector>

class HttpForkJoin
{
public:
    /**
    * 返回单例
    **/
    static HttpForkJoin *GetInstance();

    /**
    * 初始化
    **/
    int Init();

    /**
    * 销毁
    **/
    int Destroy();

    Reuzel::ThreadPool * getForkPool(){
        return m_pool;
    }

private:
    HttpForkJoin():m_pool(NULL) {
        Init();
    }
    ~HttpForkJoin() {
        Destroy();
    }

private:
    static HttpForkJoin    *m_p_instance;
    Reuzel::ThreadPool     *m_pool;

};


#endif
