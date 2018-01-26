#include "http_server.h"
#include "http_task.h"
#include <util/log.h>

#include <event.h>
#include <evhttp.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


static bool _SetAddr(const char* ip, const uint16_t port, sockaddr_in* pAddr)
{
    memset(pAddr, 0, sizeof(sockaddr_in));
    pAddr->sin_family = AF_INET;
    pAddr->sin_port = htons(port);
    //pAddr->sin_addr.s_addr = INADDR_ANY;
    pAddr->sin_addr.s_addr = inet_addr(ip);
    if (pAddr->sin_addr.s_addr == INADDR_NONE) {
        hostent* host = gethostbyname(ip);
        if (host == NULL) {
            return false;
        }
        pAddr->sin_addr.s_addr = *(uint32_t*)host->h_addr;
    }
    return true;
}

bool HttpServer::StartupServe(const char *ip,int port, int loop_count, uint32_t worker_size, uint32_t rearrange_task_waterline) {
    int ret;
    int sfd = BindSocket(ip,port);
    if (sfd < 0) {
        ERROR("Bind %s:%u failed.",ip,port);
        return false;
    }
    DEBUG("Bind %s:%u success.",ip,port);

    if(evthread_use_pthreads()!=0) {
        WARN("evthread_use_pthreads failed.");
    }

    // 初始化工作线程池
    m_thread_pool.Init(worker_size,rearrange_task_waterline);

    pthread_t threads[loop_count];
    for (int i = 0; i < loop_count; i++) {
        struct event_base *base = event_base_new();
        if (base == NULL)
            return false;

        struct evhttp *httpd = evhttp_new(base);
        if (httpd == NULL)
            return false;

        ret = evhttp_accept_socket(httpd, sfd);
        if (ret != 0)
            return false;

        evhttp_set_gencb(httpd, HttpServer::genericHandler, this);
        ret = pthread_create(&threads[i], NULL, HttpServer::dispatch, base);
        if (ret != 0)
            return false;
    }
    for (int i = 0; i < loop_count; i++) {
        pthread_join(threads[i], NULL);
    }
    return true;
}

int HttpServer::BindSocket(const char *ip,int port)
{
    int retr;
    int nfd;
    nfd = socket(AF_INET, SOCK_STREAM, 0);
    if (nfd < 0) return -1;

    int one = 1;
    retr = setsockopt(nfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(int));

    struct sockaddr_in addr;
    if(!_SetAddr(ip,port, &addr))
        return -1;

    retr = bind(nfd, (struct sockaddr*)&addr, sizeof(addr));
    if (retr < 0) return -1;
    retr = listen(nfd, 10240);
    if (retr < 0) return -1;

    int flags;
    if ((flags = fcntl(nfd, F_GETFL, 0)) < 0
            || fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;

    return nfd;
}

void* HttpServer::dispatch(void *arg) {
    event_base_dispatch((struct event_base*)arg);
    return NULL;
}

void HttpServer::genericHandler(struct evhttp_request *req, void *arg) {
    ((HttpServer*)arg)->ProcessRequest(req);
}

void HttpServer::ProcessRequest(struct evhttp_request *req) {
      HttpTask *task = new HttpTask(req);
      m_thread_pool.AddTask(task);
}
