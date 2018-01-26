#include "redis_client.h"

#include <util/cmdflags.h>
#include <util/log.h>

#include <json/json.h>
#include <string.h>

#include <fstream>
#include <chrono>
#include <thread>

enum {
 CACHE_TYPE_1, 
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};


// AP Hash Function
unsigned int APHash(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i=0; *str; i++) {
        if ((i&  1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash&  0x7FFFFFFF);
}


RedisClient * RedisClient::m_p_instance = NULL;

static CLock instance_lock;

RedisClient * RedisClient::GetInstance(){
    if(NULL == m_p_instance){   //Double-Checked Locking Pattern
        CScopeLock lock(instance_lock);
        if(NULL == m_p_instance){
            m_p_instance = new RedisClient();
        }
    }
    return m_p_instance;
}

RedisClient::RedisClient()
    :m_redisNodes(NULL),m_xRedis(NULL),m_inited(false)
{
    Init();
}

RedisClient::~RedisClient(){
    Destroy();
}


int RedisClient::Init(){

    Json::Reader reader;  
    Json::Value root;

    std::ifstream in(FLAGS_redisConfig, std::ios::binary);
    if( !in.is_open() ){
        ERROR("Cannot open %s", FLAGS_serviceConfig.c_str());
        return -1;
    }

    if(reader.parse(in,root)){
        int clusterSize = root.size();
        if(clusterSize > 0){
            m_redisNodes = new RedisNode[clusterSize];
            for(unsigned int i = 0; i < root.size(); i++)  
            {
                Json::Value & nodeElement = root[i];
                int index = nodeElement["index"].asInt();
                std::string ip = nodeElement["ip"].asString();
                int port = nodeElement["port"].asInt();
                int poolsize = nodeElement["poolsize"].asInt();
                int timeout = nodeElement["timeout"].asInt();
                (m_redisNodes+i)->dbindex = index;
                (m_redisNodes+i)->host = strdup(ip.c_str());  //leak little
                (m_redisNodes+i)->port = port;
                (m_redisNodes+i)->passwd = "";
                (m_redisNodes+i)->poolsize = poolsize;
                (m_redisNodes+i)->timeout = timeout;
                (m_redisNodes+i)->role = 0;
                INFO("Redis node : %d, %s:%d, size:%d, timeout:%d",
                    (m_redisNodes+i)->dbindex,(m_redisNodes+i)->host,(m_redisNodes+i)->port,
                    (m_redisNodes+i)->poolsize,(m_redisNodes+i)->timeout);
            }
            m_xRedis = new xRedisClient();
            m_xRedis->Init(CACHE_TYPE_MAX);
            m_xRedis->ConnectRedisCache(m_redisNodes, clusterSize, clusterSize, CACHE_TYPE_1);
            m_inited = true;
        }
    }

    in.close();
    //启动心跳检查
    StartupChecker();
    return 0;
}


int RedisClient::Destroy(){
    m_inited = false;
    delete m_xRedis;
    delete [] m_redisNodes;
    return 0;
}

void RedisClient::StartupChecker(){
    (void)pthread_create(&m_thread_id, NULL, CheckerRoutine, this);
}

void* RedisClient::CheckerRoutine(void* arg){
    RedisClient* client = (RedisClient*)arg;
	client->CheckValidation();
	return NULL;
}

void RedisClient::CheckValidation(){
    INFO("Startup CheckValidation ...");
    while(true){
        //每5分钟发一拨心跳
        std::this_thread::sleep_for(std::chrono::milliseconds(5* 60 * 1000));
        m_xRedis->Keepalive();
    }
}

bool RedisClient::Set(const std::string& key, const std::string& value, int expire){
    if(!m_inited){
        return false;
    }
    RedisDBIdx dbi(m_xRedis);
    bool bRet = dbi.CreateDBIndex(key.c_str(), APHash, CACHE_TYPE_1);
    if (!bRet) {
        return false;
    }
    bRet = m_xRedis->set(dbi, key, value.c_str(), value.length(), expire);
    if(!bRet){
        ERROR("error [%s]", dbi.GetErrInfo());
        return false;
    }
    return true;
}


bool RedisClient::Get(const std::string& key, std::string& value){
    if(!m_inited){
        return false;
    }
    RedisDBIdx dbi(m_xRedis);
    bool bRet = dbi.CreateDBIndex(key.c_str(), APHash, CACHE_TYPE_1);
    if (!bRet) {
        return false;
    }
    bRet = m_xRedis->get(dbi, key, value);
    if(!bRet){
        return false;
    }
    return true;
}
