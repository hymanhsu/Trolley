#include "log.h"
#include "cmdflags.h"
#include "util.h"

#include <string.h>  //strcmp

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

zlog_category_t *zc;
static int global_inited = 0;


int log_init_from_configfile(const char * config_file_path )
{
	if (global_inited) return 0;
	//std::cout << "log config file : " << config_file_path << std::endl;
	int rc = zlog_init(config_file_path);
	if ( rc )
	{
		std::cout << "zlog_init failed : " << rc << std::endl;
		return 0;
	}
	zc = zlog_get_category("my_cat");
	if (!zc) {
		std::cout << "zlog_get_category failed " << std::endl;
		zlog_fini();
		return 0;
	}
	global_inited = 1;
	return 1;
}

int log_init_from_flags()
{
	//write some tmp config file
	std::string configfilePath(FLAGS_workingdir);
	configfilePath += "/log.conf";
	std::stringstream ss;
	ss << "[global]" << std::endl;
	ss << "strict init = true" << std::endl;
	ss << "buffer min = 1024" << std::endl;
	ss << "buffer max = 2MB" << std::endl;
	ss << "default format = \"%D.%ms.%us %v [%p:%t:%f:%L] %m%n\"" << std::endl;
	ss << "" << std::endl;
	ss << "[formats]" << std::endl;
	ss << "normalFormat	= \"%D.%ms.%us %v [%p:%t:%f:%L] %m%n\"" << std::endl;
	ss << "" << std::endl;
	ss << "[rules]" << std::endl;
	if(FLAGS_daemonize || FLAGS_forceWriteLogfile){
		mkdirs(FLAGS_logoutdir.c_str(),0755);
		ss << "my_cat." << FLAGS_loglevel << "	\"" << FLAGS_logoutdir << "/intelli.log.%d(%F)\",1GB*5;normalFormat" << std::endl;
	}else{
		ss << "my_cat." << FLAGS_loglevel << "	>stdout;" << std::endl;
	}
	std::ofstream configfile;
	configfile.open(configfilePath, std::ios::out|std::ios::trunc);
	configfile << ss.str();
	configfile.close();
	//invoke log_init_from_configfile
	return log_init_from_configfile(configfilePath.c_str());
}

int log_fini()
{
	if (!global_inited) return 0;
	zlog_fini();
	global_inited = 0;
	return 1;
}
