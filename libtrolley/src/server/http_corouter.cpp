#include "http_corouter.h"
#include <util/log.h>

HttpCorouter * HttpCorouter::m_p_instance = NULL;

static CLock instance_lock;

HttpCorouter * HttpCorouter::GetInstance(){
    if(NULL == m_p_instance){   //Double-Checked Locking Pattern
        CScopeLock lock(instance_lock);
        if(NULL == m_p_instance){
            m_p_instance = new HttpCorouter();
        }
    }
    return m_p_instance;
}

void HttpCorouter::Process(const HttpRequest& httpRequest, HttpResponse& httpResponse){
    std::map<std::string, HttpHandler*>::iterator iter = m_handlers.find(httpRequest.getPath());
    if(iter != m_handlers.end()){
        iter->second->Process(httpRequest,httpResponse);
    }else{
        httpResponse.setCode(HTTP_NOTFOUND);
    }
}

void HttpCorouter::Register(const std::string& path, HttpHandler* handler){
    if(handler){
        m_handlers.insert (std::pair<std::string, HttpHandler*>(path,handler));
    }
}

