#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <client/http_sequence_num.h>

#include <restclient-cpp/restclient.h>
#include <restclient-cpp/connection.h>

#include <pthread.h>
#include <stdint.h>
#include <string>


class HttpClient;
class HttpRequestContext{
public:
    HttpRequestContext(uint32_t sequence,HttpClient * client):m_sequence(sequence),m_client(client){}
    ~HttpRequestContext(){}
    uint32_t getSequence() const {return m_sequence;}
    HttpClient *getClient() {return m_client;}
private:
    uint32_t     m_sequence;
    HttpClient * m_client;
};

class HttpClient{
public:
    HttpClient(std::string& ip,int port=80,uint32_t timeout=1);
    ~HttpClient();

    /**
    * 初始化
    **/
    int Init(int index=0);

    /**
    * 发送POST
    **/
    bool PostRequest(const std::string& uriPath,const std::string& postData,RestClient::Response &response);

    int getIndex() const{
        return m_index;
    }
    
private:
    int          m_index;
    std::string  m_ip;
    int          m_port;
    uint32_t     m_timeout;    //超时，秒
    RestClient::Connection      *m_conn;  //连接对象
};


#endif