#ifndef __CMDFLAGS_H__
#define __CMDFLAGS_H__

#include <gflags/gflags.h>

DECLARE_bool(daemonize);
DECLARE_bool(forceWriteLogfile);
DECLARE_string(workingdir);

DECLARE_string(logoutdir);
DECLARE_string(loglevel);

DECLARE_string(listenip);
DECLARE_uint32(listenport);
DECLARE_uint32(loopcount);
DECLARE_uint32(workernum);
DECLARE_uint32(rearrange_task_waterline);

DECLARE_string(serviceConfig);
DECLARE_uint32(fjPoolWorkerNum);
DECLARE_uint32(fjPoolQueueSize);

DECLARE_string(redisConfig);

#endif // __CMDFLAGS_H__
