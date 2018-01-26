#include "http_handler.h"
#include <util/log.h>


void EchoHandler::Process(const HttpRequest& httpRequest, HttpResponse& httpResponse){
    DEBUG("Request : %s",httpRequest.getBody().c_str());
    httpResponse.setBody(httpRequest.getBody());
}

void JsonHandler::Process(const HttpRequest& httpRequest, HttpResponse& httpResponse){
    rapidjson::Document request;
    if (request.Parse(httpRequest.getBody()).HasParseError()) {
        ERROR("Request parse json failed!");
        return;
    }
    rapidjson::Document response;
    response.SetObject();
    std::string reason;
    int code = ProcessJson(request,response,reason);
    httpResponse.setCode(code);
    httpResponse.setReason(reason);
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    response.Accept(writer);
    std::string responseBodyText = strbuf.GetString();
    httpResponse.setBody(responseBodyText);
}

