#ifndef __HTTP_SEQUENCE_NUM_H__
#define __HTTP_SEQUENCE_NUM_H__

#include <util/lock.h>

#include <stdint.h>


class HttpSequenceNum
{
public:
    /**
    * 返回单例
    **/
    static HttpSequenceNum *GetInstance();

    /**
    * 返回下一个序号，范围：[1,Max_value_of_uint32]
    **/
    uint32_t next();
private:
    HttpSequenceNum():m_sequence_num(0) {}
    ~HttpSequenceNum() {}
private:
    static HttpSequenceNum *m_p_instance;
    CLock                   m_lock;
    uint32_t                m_sequence_num;
};


#endif
