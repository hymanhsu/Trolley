#include "http_task.h"
#include "http_corouter.h"
#include <util/log.h>

#include <event.h>
#include <evhttp.h>
#include <event2/thread.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/buffer_compat.h>

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>


#define MAX_REQUEST_SIZE 1024

HttpTask::HttpTask(struct evhttp_request *req)
    : m_req(req)
{
}

HttpTask::~HttpTask()
{
}

void HttpTask::run()
{
    HttpRequest httpRequest;

    //Read path
    const struct evhttp_uri * httpuri = evhttp_request_get_evhttp_uri(m_req);
    if(!httpuri){
        ERROR("evhttp_request_get_evhttp_uri failed");
        return;
    }
    const char * path = evhttp_uri_get_path(httpuri);
    //DEBUG("request path : %s", path);
    httpRequest.setPath(path);

    // Get Input buffer
    struct evbuffer *buf_input = evhttp_request_get_input_buffer(m_req);
    if(!buf_input) {
        ERROR("evhttp_request_get_input_buffer failed");
        return;
    }

    //Read body
    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
    evbuffer_enable_locking(buf_input,NULL);
    // Lock buffer when multi-thread
    evbuffer_lock(buf_input);
    size_t length = EVBUFFER_LENGTH(m_req->input_buffer);
    DEBUG("request length : %ld",length);
    if(length>0){
        char fixed_request_body[MAX_REQUEST_SIZE+1];
        char * request_body = NULL;
        if(length > MAX_REQUEST_SIZE){
            DEBUG("MAX_REQUEST_SIZE is not enough , real length is %d", length);
            request_body = (char *)calloc(length+1,1);
            evbuffer_remove(buf_input,request_body,length);
            *(request_body+length) = 0;
            httpRequest.setBody(request_body);
        }else{
            evbuffer_remove(buf_input,fixed_request_body,length);
            *(fixed_request_body+length) = 0;
            httpRequest.setBody(fixed_request_body);
        }
        if(request_body){
            free(request_body);
        }
    }
    evbuffer_unlock(buf_input);

    //Invoke corouter
    HttpResponse httpResponse;
    HttpCorouter::GetInstance()->Process(httpRequest,httpResponse);

    // Get Output buffer
    struct evbuffer *buf_output = evhttp_request_get_output_buffer(m_req);
    if(!buf_output) {
        ERROR("evhttp_request_get_output_buffer failed");
        return;
    }

    // Enable locking on an evbuffer so that it can safely be used by multiple threads at the same time.
    evbuffer_enable_locking(buf_output,NULL);
    // Lock buffer when multi-thread
    evbuffer_lock(buf_output);
    evhttp_add_header(m_req->output_headers, "Content-Type", "application/json; charset=UTF-8");
    if(httpResponse.getCode() == HTTP_OK){
        const std::string & responseBody = httpResponse.getBody();
        evbuffer_add(buf_output,responseBody.c_str(),responseBody.length());
        evhttp_send_reply(m_req, httpResponse.getCode(), httpResponse.getReason().c_str(), buf_output);
    }else{
        evhttp_send_error(m_req, httpResponse.getCode(), httpResponse.getReason().c_str());
        //evbuffer_add_printf(buf_output, "Error reason : %s", httpResponse.getReason().c_str());
        //evhttp_send_reply(m_req, httpResponse.getCode(), httpResponse.getReason().c_str(), buf_output);
    }
    evbuffer_unlock(buf_output);
}

