#ifndef __HTTP_COROUTER_H__
#define __HTTP_COROUTER_H__


#include <server/http_handler.h>
#include <util/lock.h>

#include <evhttp.h>

#include <unistd.h>
#include <string>
#include <map>


class HttpCorouter
{
public:
    /**
    * 返回单例
    **/
    static HttpCorouter *GetInstance();

    /**
    * 处理请求，填充应答
    **/
    void Process(const HttpRequest& httpRequest, HttpResponse& httpResponse);

    /**
    * 注册URI的处理器
    **/
    void Register(const std::string& path, HttpHandler* handler);
private:
    HttpCorouter() {}
    ~HttpCorouter() {}
private:
    static HttpCorouter *m_p_instance;
    std::map<std::string, HttpHandler*> m_handlers; 
};

#endif