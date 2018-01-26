/************************
 * 工具
************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <sys/stat.h>  //mode_t
#include <sys/time.h>  //timeval

/**
 * 守护进程化
**/
void daemonize (const char * cwd);

/**
** 创建多级目录
**/
int mkdirs (const char * dir_path, mode_t mode);

/**
** 文件路径是否存在
**/
int is_path_exist(const char * file_path);

#endif
