#ifndef __HTTP_CLIENT_POOL_H__
#define __HTTP_CLIENT_POOL_H__

#include <client/http_client.h>

#include <util/lock.h>

#include <stdint.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>


class HttpClientPool{
public:
    HttpClientPool(std::string& ip,int port=80,uint32_t timeout=1,int max_num=10);

    ~HttpClientPool();

    HttpClient* RequireConnection();

    void ReleaseConnection(HttpClient* conn);

    void Init();

    void Destroy();

    const char* getName() const{
        return m_key.c_str();
    }

private:

	//获取可用连接数量
	int get_available_conn_num()
	{
		return m_unused_conn_list.size();
	}
	//获取总连接数量
	int get_total_conn_num()
	{
		return m_unused_conn_list.size() + m_used_conn_list.size();
	}
	//获取锁
    void lock()
    {
    	m_lock.use();
    }
	//释放锁
    void unlock()
    {
    	m_lock.unuse();
    }

private:
    CLock                             m_lock;
	std::vector<HttpClient *>         m_unused_conn_list;   //空闲连接队列
	std::vector<HttpClient *>         m_used_conn_list;     //已用连接队列

    int                               m_index;
    std::string                       m_key;
    std::string                       m_ip;
    int                               m_port;
    uint32_t                          m_timeout;    //超时，秒
    int                               m_max_num;  //最大连接对象数量
    bool                              m_inited;
};



#endif
