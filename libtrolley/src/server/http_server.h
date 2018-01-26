#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <server/thread_pool.h>

class HttpServer
{
public:
    HttpServer() {}
    ~HttpServer() {}

    /**
    * 启动
    **/
    bool StartupServe(const char *ip="0.0.0.0", int port=18000, int loop_count=2, uint32_t worker_size=6, uint32_t rearrange_task_waterline=10);

private:
    /**
    * 运行event_base的线程的入口函数
    **/
    static void* dispatch(void *arg);

    /**
    * libevent的请求反调函数
    **/
    static void genericHandler(struct evhttp_request *req, void *arg);

    /**
    * 处理请求
    **/
    void ProcessRequest(struct evhttp_request *request);

    /**
    * 创建侦听套接字
    **/
    int BindSocket(const char *ip, int port);

    ThreadPool m_thread_pool;

};
#endif // HTTPSERVER_H
