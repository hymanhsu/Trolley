#include "http_sequence_num.h"

HttpSequenceNum * HttpSequenceNum::m_p_instance = NULL;

static CLock instance_lock;

HttpSequenceNum * HttpSequenceNum::GetInstance(){
    if(NULL == m_p_instance){   //Double-Checked Locking Pattern
        CScopeLock lock(instance_lock);
        if(NULL == m_p_instance){
            m_p_instance = new HttpSequenceNum();
        }
    }
    return m_p_instance;
}

uint32_t HttpSequenceNum::next(){
    CScopeLock lock(m_lock);
    m_sequence_num++;
    if(0 == m_sequence_num){
        m_sequence_num++;
    }
    return m_sequence_num;
}

