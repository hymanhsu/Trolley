#ifndef __HTTP_SERVICE_MGR_H__
#define __HTTP_SERVICE_MGR_H__


#include <client/http_client_pool.h>
#include <util/lock.h>

#include <stdint.h>

#include <string>
#include <map>
#include <vector>


class HttpServiceMgr
{
public:

    typedef std::vector<HttpClientPool*>     PoolList;
    typedef std::map<std::string,PoolList*>  ServicePoolMap;

    /**
    * 返回单例
    **/
    static HttpServiceMgr *GetInstance();

    /**
    * 通过服务名称，请求服务
    **/
    bool PostRequest(const std::string& serviceName,const std::string& uriPath,const std::string& postData,RestClient::Response &response);

private:
    HttpServiceMgr();
    ~HttpServiceMgr();

    void RegisterPool(const std::string& serviceName, HttpClientPool* pool);
private:
    static HttpServiceMgr  *m_p_instance;
    CLock                   m_lock;
    ServicePoolMap          m_service_pool;
};


#endif
