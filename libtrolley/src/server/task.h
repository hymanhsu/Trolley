#ifndef __COMMONTASK_H__
#define __COMMONTASK_H__

class CommonTask {
public:
    CommonTask(){}
    virtual ~CommonTask(){}
    virtual void run() = 0;
};

#endif /*defined(__COMMONTASK_H__) */
