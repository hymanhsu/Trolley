#include <util/log.h>
#include <util/util.h>
#include <util/cmdflags.h>
#include <server/http_server.h>
#include <server/http_corouter.h>
#include <client/http_client.h>
#include <client/http_client_pool.h>
#include <client/http_service_mgr.h>
#include <client/http_forkjoin.h>

#include <unistd.h>  //sleep,write
#include <string.h> //memcpy
#include <sys/types.h> 
#include <sys/socket.h> //socketpair 
#include <sys/dir.h>  //PATH_MAX
#include <stdlib.h>   //atoi
#include <stdio.h>
#include <getopt.h>
#include <ctype.h> 
#include <signal.h>  //sigaction

#include <chrono>
#include <thread>
#include <pthread.h>


class JsonexampleHandler : public JsonHandler
{
public:
    JsonexampleHandler(){}
    virtual ~JsonexampleHandler(){}

    /**
    * 处理JSON请求
    **/
    int ProcessJson(const rapidjson::Document & request, rapidjson::Document & response, std::string & reason, const std::string& rawRequest){
		if(request.HasMember("key")){
			int sleep_ms = request["sleep"].GetInt();
			std::string strValue = request["key"].GetString();
			rapidjson::Document::AllocatorType& allocator = response.GetAllocator();
			response.AddMember("answer", strValue, allocator);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
		}
		return HTTP_OK;
	}

};

int server(){
	HttpCorouter::GetInstance()->Register("/echo",new EchoHandler());
	HttpCorouter::GetInstance()->Register("/jsonexample/test",new JsonexampleHandler());
	HttpServer httpServer;
	httpServer.StartupServe(FLAGS_listenip.c_str(),FLAGS_listenport,FLAGS_loopcount,FLAGS_workernum,FLAGS_rearrange_task_waterline);
	return 0;
}


int main(int argc, char** argv){
	// Parse gflags. We recommend you to use gflags as well.
    gflags::ParseCommandLineFlags(&argc, &argv, true);
	//create working directory
	mkdirs(FLAGS_workingdir.c_str(),0755);
	//daemonize
    if(FLAGS_daemonize){
        daemonize(FLAGS_workingdir.c_str());
    }
    if(!log_init_from_flags()){
        return -1;
    }
	return server();
}

