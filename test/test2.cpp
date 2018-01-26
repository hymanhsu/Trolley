#include <util/log.h>
#include <util/util.h>
#include <util/cmdflags.h>
#include <server/http_server.h>
#include <server/http_corouter.h>
#include <client/http_client.h>
#include <client/http_client_pool.h>
#include <client/http_service_mgr.h>
#include <client/http_forkjoin.h>
#include <redis/redis_client.h>

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
#include <string>


#define random(x) (rand()%x)

void *workerTest(void * arg)
{
	DEBUG("begin to test ...");
    int val = 100;
	while(val > 0){
		char key[20];
		snprintf(key, sizeof(key), "key_%d", val);
        int r = random(10);
		char value[100];
		snprintf(value, sizeof(value), "value_%d", r);
        DEBUG("set value [%s] for %s",value,key);
        RedisClient::GetInstance()->Set(key,value,r);
        //int sleepSecs = random(20);
        //DEBUG("sleep %d s for %s",sleepSecs,key);
        std::this_thread::sleep_for(std::chrono::milliseconds(5* 1000));
        std::string valueStr;
        if(RedisClient::GetInstance()->Get(key,valueStr)){
            DEBUG("got value [%s] for %s",valueStr.c_str(),key);
        }else{
            DEBUG("not found value for %s",key);
        }
        val--;
	}
    DEBUG("end to test !!!");
}

int client(){	
    //RedisClient::GetInstance()->Init();

    //getchar();

	pthread_t ntid1; 
	pthread_create(&ntid1, NULL, workerTest, NULL); 

	pthread_t ntid2; 
	pthread_create(&ntid2, NULL, workerTest, NULL); 

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
    client();
}
