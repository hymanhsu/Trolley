#include "http_client_pool.h"
#include <util/cmdflags.h>
#include <util/log.h>

#include <chrono>
#include <thread>

HttpClientPool::HttpClientPool(std::string& ip,int port,uint32_t timeout,int max_num) 
    :m_max_num(max_num),m_ip(ip),m_port(port),m_timeout(timeout)
{
    char key[100];
    snprintf(key, sizeof(key), "%s:%d", m_ip.c_str(), m_port);
    m_key = key;
    m_index = 0;
}
    

HttpClientPool::~HttpClientPool() 
{
    Destroy();
}


void HttpClientPool::Init(){
    m_inited = true;
}

void HttpClientPool::Destroy()
{
	while (!m_used_conn_list.empty())
	{
        WARN ("Destroy ... used conn list is not empty , wait 100 ms");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if(m_inited)
	{
		lock();
		while (!m_unused_conn_list.empty())
		{
			HttpClient * conn = m_unused_conn_list.back();
			m_unused_conn_list.pop_back();
			delete conn;
		}
		unlock();
	}
}


//获取连接
HttpClient* HttpClientPool::RequireConnection()
{
    if(!m_inited){
        return NULL;
    }
	HttpClient * conn = NULL;
	lock();
	if (!m_unused_conn_list.empty())
	{
		conn = m_unused_conn_list.back();
		//是否需要每次取连接之前检查连接的有效性呢???
		m_unused_conn_list.pop_back();
		m_used_conn_list.push_back(conn);
	}
	else
	{
		//如果连接已经不够用，根据规则进行新连接的申请
		if (get_total_conn_num() < m_max_num)
		{
            conn = new HttpClient(m_ip,m_port);
            conn->Init(++m_index);
            m_used_conn_list.push_back(conn);
		}
		else
		{
			ERROR ("require connection failed : exceed max conn num");
		}
	}
	unlock();
	if(conn)
	{
		//DEBUG ( "require connection [%s,%d] sucessfully", getName(), conn->getIndex() );
	}
	else
	{
		//ERROR ( "require connection failed" );
	}	
	return conn;
}


//释放连接
void HttpClientPool::ReleaseConnection(HttpClient* conn)
{
    if(!m_inited){
        return;
    }
	if (conn==NULL) 
        return;
	lock();
	for( int i = 0; i< m_used_conn_list.size(); ++i)
	{
		if ( m_used_conn_list[i] == conn )
		{
			std::vector<HttpClient *>::iterator iter = m_used_conn_list.begin()+i;
			//DEBUG ( "release connection [%s,%d]", getName(), (*iter)->getIndex() );
			m_used_conn_list.erase ( iter );
			break;
		}
	}
	m_unused_conn_list.push_back(conn);
	unlock();
}

