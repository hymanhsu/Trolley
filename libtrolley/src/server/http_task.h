#ifndef __HTTP_TASK_H__
#define __HTTP_TASK_H__

#include <server/task.h>

#include <evhttp.h>

class HttpTask : public Task
{
public:
    HttpTask(struct evhttp_request *req);
    ~HttpTask();
    void run();
private:
    struct evhttp_request *m_req;
};

#endif // __HTTP_TASK_H__
