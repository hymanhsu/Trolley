#include "cmdflags.h"


DEFINE_bool(daemonize, false, "Daemonize mode, default is False");
DEFINE_bool(forceWriteLogfile, false, "Force to write log to file, default is False");
DEFINE_string(workingdir, "/tmp/intelli/", "Working directory, default is '/tmp/intelli/' ");

DEFINE_string(logoutdir, "/tmp/intelli/logs/", "The output directory which write logs, default is '/tmp/logs/' ");
DEFINE_string(loglevel, "DEBUG", "Log level, default is 'DEBUG' ");

DEFINE_string(listenip, "0.0.0.0", "Listen ip, default is '0.0.0.0' ");
DEFINE_uint32(listenport, 18000, "Listen port, default is 18000");
DEFINE_uint32(loopcount, 2, "Loop count, default is 2");
DEFINE_uint32(workernum, 6, "Worker number, default is 6");
DEFINE_uint32(rearrange_task_waterline, 10, "Rearrange task waterline, default is 10");

DEFINE_string(serviceConfig, "/etc/ia_services.json", "Service config file path, default is '/etc/ia_services.json' ");
DEFINE_uint32(fjPoolWorkerNum, 20, "ForkJoin pool worker number, default is 20");
DEFINE_uint32(fjPoolQueueSize, 5000, "ForkJoin pool queue size, default is 5000");

DEFINE_string(redisConfig, "/etc/redis_cluster.json", "Redis config file path, default is '/etc/redis_cluster.json' ");

