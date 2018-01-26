/************************
 * 日志
************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h> 

#include <zlog.h>

extern zlog_category_t *zc;

/**
 * 从配置文件初始化日志配置
**/
int log_init_from_configfile(const char * config_file_path);

/**
 * 从命令行参数初始化日志配置
 **/
int log_init_from_flags();

/**
 * 清理日志设施
 **/
int log_fini();

/**
 * 日志输出宏
**/
#define DEBUG(logFmt,args...)   zlog_debug(zc, logFmt, ##args)
#define INFO(logFmt,args...)    zlog_info(zc, logFmt, ##args)
#define NOTICE(logFmt,args...)  zlog_notice(zc, logFmt, ##args)
#define WARN(logFmt,args...)    zlog_info(zc, logFmt, ##args)
#define ERROR(logFmt,args...)   zlog_error(zc, logFmt, ##args)
#define FATAL(logFmt,args...)   zlog_fatal(zc, logFmt, ##args)

#define HEXDEBUG(msg,len)   hzlog_debug(zc,msg,len)
#define HEXINFO(msg,len)    hzlog_info(zc,msg,len)
#define HEXNOTICE(msg,len)  hzlog_notice(zc,msg,len)
#define HEXWARN(msg,len)    hzlog_info(zc,msg,len)
#define HEXERROR(msg,len)   hzlog_error(zc,msg,len)
#define HEXFATAL(msg,len)   hzlog_fatal(zc,msg,len)

#endif
