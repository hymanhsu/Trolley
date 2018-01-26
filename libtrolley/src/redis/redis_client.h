#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__


#include <util/lock.h>
#include <xRedisClient.h>

#include <stdint.h>
#include <pthread.h>

#include <string>
#include <map>
#include <vector>


class RedisClient
{
public:
    /**
    * 返回单例
    **/
    static RedisClient *GetInstance();

    int Init();

    int Destroy();

    /**
    * 设置值
    **/
    bool Set(const std::string& key, const std::string& value, int expire=0);

    /**
    * 取值,如果返回false，则表示没有找到数据
    **/
    bool Get(const std::string& key, std::string& value);

private:
    RedisClient();
    ~RedisClient();

    void CheckValidation();

    void StartupChecker();

    static void* CheckerRoutine(void* arg);

private:
    static RedisClient     *m_p_instance;
    CLock                   m_lock;
    RedisNode              *m_redisNodes;
    xRedisClient           *m_xRedis;
    bool                    m_inited;
    pthread_t		        m_thread_id;
};


#endif
