#include "flags.h"

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


#define random(x) (rand()%x)

int client(){
	RestClient::init();

	HttpClientPool * pool = new HttpClientPool(FLAGS_remoteip,FLAGS_remoteport);
	pool->Init();

	
	DEBUG("begin to test ...");
	while(true){
		RestClient::Response response;
		char reqBuf[1024];
		snprintf(reqBuf, sizeof(reqBuf), "{\"sleep\":%d,\"key\":\"test\"}", random(2000));
		DEBUG("Request:%s",reqBuf);
		HttpClient * client = pool->RequireConnection();
		if(client){
			if(client->PostRequest("/jsonexample/test",reqBuf,response)){
				DEBUG("Code:%d",response.code);
				DEBUG("Body:%s",response.body.c_str());
			}
			pool->ReleaseConnection(client);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}

	RestClient::disable();
	return 0;
}

void *workerTest(void * arg)
{
	HttpClientPool * pool = (HttpClientPool *)arg;
	DEBUG("begin to test ...");
	while(true){
		RestClient::Response response;
		char reqBuf[1024];
		snprintf(reqBuf, sizeof(reqBuf), "{\"sleep\":%d,\"key\":\"test\"}", random(2000));
		DEBUG("Request:%s",reqBuf);
		HttpClient * client = pool->RequireConnection();
		if(client){
			if(client->PostRequest("/jsonexample/test",reqBuf,response)){
				DEBUG("Code:%d",response.code);
				DEBUG("Body:%s",response.body.c_str());
			}
			pool->ReleaseConnection(client);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int client2(){
	RestClient::init();

	HttpClientPool * pool = new HttpClientPool(FLAGS_remoteip,FLAGS_remoteport);
	pool->Init();
	
	pthread_t ntid1; 
	pthread_create(&ntid1, NULL, workerTest, pool); 

	pthread_t ntid2; 
	pthread_create(&ntid2, NULL, workerTest, pool); 

	int **ret;
	pthread_join(ntid1,(void**)ret);
	pthread_join(ntid2,(void**)ret);

	RestClient::disable();
	return 0;
}

void *workerTest3(void * arg)
{
	DEBUG("begin to test ...");
	while(true){
		RestClient::Response response;
		char reqBuf[1024];
		snprintf(reqBuf, sizeof(reqBuf), "{\"sleep\":%d,\"key\":\"test\"}", random(2000));
		DEBUG("Request:%s",reqBuf);
		if(HttpServiceMgr::GetInstance()->PostRequest("TEST","/jsonexample/test",reqBuf,response)){
			DEBUG("Code:%d",response.code);
			DEBUG("Body:%s",response.body.c_str());
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int client3(){
	RestClient::init();
	
	pthread_t ntid1; 
	pthread_create(&ntid1, NULL, workerTest3, NULL); 

	pthread_t ntid2; 
	pthread_create(&ntid2, NULL, workerTest3, NULL); 

	int **ret;
	pthread_join(ntid1,(void**)ret);
	pthread_join(ntid2,(void**)ret);

	RestClient::disable();
	return 0;
}

void *workerTest4(void * arg)
{
	DEBUG("begin to test ...");
	while(true){
		//每次并行发出两个请求
		Reuzel::CountDownLatch latch(2);

		char reqBuf1[1024];
		snprintf(reqBuf1, sizeof(reqBuf1), "{\"sleep\":%d,\"key\":\"test\"}", random(2000));
		char reqBuf2[1024];
		snprintf(reqBuf2, sizeof(reqBuf2), "{\"sleep\":%d,\"key\":\"test\"}", random(2000));

		RestClient::Response response1;
		RestClient::Response response2;

		HttpForkJoin::GetInstance()->getForkPool()->addTask(
			[&](){
				if(HttpServiceMgr::GetInstance()->PostRequest("TEST","/jsonexample/test",reqBuf1,response1)){
				}
				latch.countDown(); 
			}
		);

		HttpForkJoin::GetInstance()->getForkPool()->addTask(
			[&](){
				if(HttpServiceMgr::GetInstance()->PostRequest("TEST","/jsonexample/test",reqBuf2,response2)){
				}
				latch.countDown(); 
			}
		);

		latch.wait();

		//同时打印两个结果
        DEBUG("Request1:%s",reqBuf1);
		DEBUG("Request1 Response Code:%d",response1.code);
		DEBUG("Request1 Response Body:%s",response1.body.c_str());
        DEBUG("Request2:%s",reqBuf2);
		DEBUG("Request2 Response Code:%d",response2.code);
		DEBUG("Request2 Response Body:%s",response2.body.c_str());

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int client4(){
	pthread_t ntid1; 
	pthread_create(&ntid1, NULL, workerTest4, NULL); 

	pthread_t ntid2; 
	pthread_create(&ntid2, NULL, workerTest4, NULL); 

	int **ret;
	pthread_join(ntid1,(void**)ret);
	pthread_join(ntid2,(void**)ret);

	return 0;
}


int main(int argc, char** argv){
	// Parse gflags. We recommend you to use gflags as well.
    gflags::ParseCommandLineFlags(&argc, &argv, true);
	//create working directory
	mkdirs(FLAGS_workingdir.c_str(),0755);
    if(!log_init_from_flags()){
        return -1;
    }
    client4();
}
