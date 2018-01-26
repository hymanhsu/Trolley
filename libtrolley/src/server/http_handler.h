#ifndef __HTTP_HANDLER_H__
#define __HTTP_HANDLER_H__

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <evhttp.h>

#include <string>

class HttpRequest{
public:
    HttpRequest(){}
    HttpRequest(const std::string& path, const std::string& body):m_path(path),m_body(body){}
    ~HttpRequest(){}
    void setPath(const std::string& path){
        m_path = path;
    }
    const std::string& getPath() const{
        return m_path;
    }
    void setBody(const std::string& body){
        m_body = body;
    }
    const std::string& getBody() const{
        return m_body;
    }
private:
    std::string m_path;
    std::string m_body;
};

class HttpResponse{
public:
    HttpResponse():m_code(HTTP_OK),m_reason(""),m_body(""){}
    HttpResponse(const std::string& body):m_body(body){}
    ~HttpResponse(){}
    void setBody(const std::string& body){
        m_body = body;
    }
    const std::string& getBody() const{
        return m_body;
    }
    void setCode(int code){
        m_code = code;
    }
    int getCode() const {
        return m_code;
    }
    void setReason(const std::string& reason){
        m_reason = reason;
    }
    const std::string& getReason() const{
        return m_reason;
    }
private:
    int         m_code;
    std::string m_reason;
    std::string m_body;
};

class HttpHandler {
public:
    HttpHandler(){}
    virtual ~HttpHandler(){}
    /**
    * 处理HTTP请求
    **/
    virtual void Process(const HttpRequest& httpRequest, HttpResponse& httpResponse) = 0;
};

class EchoHandler : public HttpHandler
{
public:
    EchoHandler(){}
    ~EchoHandler(){}
    void Process(const HttpRequest& httpRequest, HttpResponse& httpResponse);
};

class JsonHandler : public HttpHandler
{
public:
    JsonHandler(){}
    virtual ~JsonHandler(){}
    void Process(const HttpRequest& httpRequest, HttpResponse& httpResponse);

    /**
    * 处理JSON请求
    **/
    virtual int ProcessJson(const rapidjson::Document & request, rapidjson::Document & response, std::string & reason) = 0;
};

#endif