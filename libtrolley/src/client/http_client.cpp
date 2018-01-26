#include "http_client.h"

#include <util/log.h>

#include <string.h>
#include <stdlib.h>


HttpClient::HttpClient(std::string& ip,int port,uint32_t timeout):
    m_ip(ip),m_port(port),m_timeout(timeout),m_conn(NULL)
{    
}

HttpClient::~HttpClient(){
    delete m_conn;
}

int HttpClient::Init(int index){
    m_index = index;
    char endpoint[1024];
    snprintf(endpoint, sizeof(endpoint), "http://%s:%d", m_ip.c_str(), m_port);
    m_conn = new RestClient::Connection(endpoint);
    m_conn->SetTimeout(m_timeout);
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "application/json; charset=UTF-8";
    headers["Accept"] = "application/json";
    m_conn->SetHeaders(headers);
    return 0;
}

bool HttpClient::PostRequest(const std::string& uriPath,const std::string& postData,RestClient::Response &response){
    if(NULL==m_conn){
        ERROR("Connection is not valid");
        return false;
    }
    response = m_conn->post(uriPath.c_str(), postData);
    return true;
}

