#include "http_service_mgr.h"
#include <util/cmdflags.h>
#include <util/log.h>

#include <json/json.h> 

#include <fstream>



HttpServiceMgr * HttpServiceMgr::m_p_instance = NULL;

static CLock instance_lock;

HttpServiceMgr * HttpServiceMgr::GetInstance(){
    if(NULL == m_p_instance){   //Double-Checked Locking Pattern
        CScopeLock lock(instance_lock);
        if(NULL == m_p_instance){
            m_p_instance = new HttpServiceMgr();
        }
    }
    return m_p_instance;
}

HttpServiceMgr::HttpServiceMgr(){
    Json::Reader reader;  
    Json::Value root;

    std::ifstream in(FLAGS_serviceConfig, std::ios::binary);
    if( !in.is_open() ){
        ERROR("Cannot open %s", FLAGS_serviceConfig.c_str());
        return;
    }

    if(reader.parse(in,root)){
        for(unsigned int i = 0; i < root.size(); i++)  
        {  
            Json::Value & serviceElement = root[i];
            std::string serviceName = serviceElement["serviceName"].asString();
            Json::Value & endpointsElement = serviceElement["endpoints"];
            for(unsigned int j = 0; j < endpointsElement.size(); j++)
            {
                std::string ip = endpointsElement[j]["ip"].asString();
                int port = endpointsElement[j]["port"].asInt();
                int timeout = endpointsElement[j]["timeout"].asInt();
                int poolSize = endpointsElement[j]["poolSize"].asInt();
                HttpClientPool * pool = new HttpClientPool(ip,port,timeout,poolSize);
                pool->Init();
                RegisterPool(serviceName,pool);
                INFO("Register Service: %s, Endpoint: %s,%d, Timeout: %d, PoolSize: %d", serviceName.c_str(),ip.c_str(),port,timeout,poolSize );
            }
        } 
    }

    in.close();

    //全局初始化curl
    RestClient::init();
}

HttpServiceMgr::~HttpServiceMgr(){
    //全局销毁curl
    RestClient::disable();
}

void HttpServiceMgr::RegisterPool(const std::string& serviceName, HttpClientPool* pool){
    if(NULL == pool){
        return;
    }
    CScopeLock lock(m_lock);
    ServicePoolMap::iterator iter = m_service_pool.find(serviceName);
    if(iter != m_service_pool.end()){
        PoolList* list = iter->second;
        list->push_back(pool);
    }else{
        PoolList* list = new PoolList();
        list->push_back(pool);
        m_service_pool.insert (std::pair<std::string, PoolList*>(serviceName,list));
    }
}


bool HttpServiceMgr::PostRequest(const std::string& serviceName,const std::string& uriPath,const std::string& postData,RestClient::Response &response){
    ServicePoolMap::iterator iter = m_service_pool.find(serviceName);
    if(iter != m_service_pool.end()){
        PoolList* list = iter->second;
        for (PoolList::iterator iter2 = list->begin() ; iter2 != list->end(); ++iter2){
            HttpClientPool* pool = *iter2;
            HttpClient* client = pool->RequireConnection();
            if(client){
                DEBUG("Found '%s' connection %s, %d",serviceName.c_str(),pool->getName(),client->getIndex());
                bool ret = false;
                if(client->PostRequest(uriPath,postData,response)){
                    ret = true;
                }
                pool->ReleaseConnection(client);
                return ret;
            }
        }
    }
    WARN("Not found available connection of %s", serviceName.c_str());
    return false;
}

