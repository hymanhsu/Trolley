#include "util.h"
#include "log.h"

#include <unistd.h>   //access,getpid
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <sys/stat.h>  //umask,mkdir

#include <signal.h>   //SIG_IGN
#include <stdlib.h>  
#include <string.h>   //strlen,strerror
#include <errno.h>   //errno
#include <stdio.h>    //fopen

#include <netinet/in.h>  //sockaddr_in
#include <arpa/inet.h>  //inet_ntoa
#include <sys/socket.h> //socketpair,recv

#include <fcntl.h>   //fcntl
#include <sys/ioctl.h>  //ioctl

#include <string>

void daemonize(const char * cwd)
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
	{
		printf ("can't get file limit");
		exit(0);
	}

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
	{
		printf ("can't fork");
		exit(0);
	}		
	else if (pid != 0) /* parent */
		exit(0);
	
	/*
	* Set new session 
	*/
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
	{
		printf ("can't ignore SIGHUP");
		exit(0);
	}
	
	if ((pid = fork()) < 0)
	{
		printf ("can't fork");
		exit(0);
	}
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir(cwd) < 0)
	{
		printf ("can't change directory to %s",cwd);
		exit(0);
	}
	
	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);
}


int mkdirs (const char * dir_path, mode_t mode)
{
	char * ptr = const_cast<char *>(dir_path);
	int len = strlen(dir_path);
	int index = 0;
	int sepPos = -1;
	while ( index < len )
	{
		if ( ptr[index]=='/' && index != 0 )
		{
			sepPos = index;
			std::string strDir(ptr,index);
			if (!is_path_exist(strDir.c_str()))
			{
				if(mkdir(strDir.c_str(),mode)==0)
				{
				}
				else
				{
					ERROR ("mkdirs %s failed : %s",strDir.c_str(),strerror(errno));
					return 0;
				}
			}
		}
		index++;
	}
	if ( sepPos != index-1 )
	{
		//表明路径不是以'/'结束的
		if (!is_path_exist(dir_path))
		{
			if(mkdir(dir_path,mode)==0)
			{
			}
			else
			{
				ERROR ("mkdirs %s failed : %s",dir_path,strerror(errno));
				return 0;
			}
		}
	}
	return 1;
}


int is_path_exist(const char * file_path)
{
	if ( access(file_path,F_OK) == 0 )
	{
  		return 1;
  	}
  	else
  	{	
		return 0;
	}
}

