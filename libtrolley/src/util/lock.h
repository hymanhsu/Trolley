#ifndef __LOCK_H__
#define __LOCK_H__

#include <pthread.h>

class CLock {
public:
  //constructor
  CLock() {
    //default mutex attr
    pthread_mutex_init(&m_lkMutex, NULL);
  }

  //destructor
  virtual ~CLock() {
    pthread_mutex_destroy(&m_lkMutex);
  }

public:
  //try to lock the mutex
  bool tryuse() {
    return (pthread_mutex_trylock(&m_lkMutex) == 0) ? true : false;
  }

  //lock the mutex
  bool use() {
    return (pthread_mutex_lock(&m_lkMutex) == 0) ? true : false;
  }

  //unlock the mutex
  bool unuse() {
    return (pthread_mutex_unlock(&m_lkMutex) == 0) ? true : false;
  }

  //get the mutex ref 
  pthread_mutex_t &self() {
     return m_lkMutex;
  }

private:
  //mutex var
  pthread_mutex_t m_lkMutex;
};

class CScopeLock {
public:
  //constructor
  CScopeLock(CLock &locker): m_slLock(locker) {
    m_slLock.use();
  }

  virtual ~CScopeLock() {
   m_slLock.unuse();
  }
  
private:
  CLock &m_slLock;
};

//
// Read-Write Lock
//
class CRWLock {
public:
   //constructor
   CRWLock() {
      //default lock attr
      pthread_rwlock_init(&m_rwLock, NULL);
   }

   //destructor
   virtual ~CRWLock() {
      pthread_rwlock_destroy(&m_rwLock);
   }

public:
   //lock read lock
   bool useRD() {
      return (pthread_rwlock_rdlock(&m_rwLock) == 0) ? true : false;
   }

   //try to lock read lock
   bool tryRD() {
      return (pthread_rwlock_tryrdlock(&m_rwLock) == 0) ? true : false;
   }

   //lock write lock
   bool useWR() {
      return (pthread_rwlock_wrlock(&m_rwLock) == 0) ? true : false;
   }

   //lock write lock
   bool tryWR() {
      return (pthread_rwlock_trywrlock(&m_rwLock) == 0) ? true : false;
   }

   //unlock read-write lock
   bool unuse() {
      return (pthread_rwlock_unlock(&m_rwLock) == 0) ? true : false;
   }

private:
   //read-write lock
   pthread_rwlock_t m_rwLock;
};

//
// Read Scope lock
//
class CScopeRDLock {
public:
   //constructor
   CScopeRDLock(CRWLock &rdlock) 
   : m_srdLock(rdlock)
   {
      m_srdLock.useRD();
   }

   //destructor
   virtual ~CScopeRDLock() {
      m_srdLock.unuse();
   }

private:
   //read lock
   CRWLock &m_srdLock;
};

//
// write scope lock
//
class CScopeWRLock {
public:
   //constructor
   CScopeWRLock(CRWLock &wrlock) 
   : m_swrLock(wrlock)
   {
      m_swrLock.useWR();
   }

   //destructor
   virtual ~CScopeWRLock() {
      m_swrLock.unuse();
   }

private:
   //read lock
   CRWLock &m_swrLock;
};

//
// Conditional Variable
//
class CCondMutex {
public:
   //constructor
   CCondMutex() {
      //default cond attrib
      pthread_cond_init(&m_cmCondVar, NULL);
   }

   //destructor
   virtual ~CCondMutex() {
      pthread_cond_destroy(&m_cmCondVar);
   }

public:
   //wait for signal
   bool wait(CLock &lock) {
      return (pthread_cond_wait(&m_cmCondVar, &lock.self()) == 0) ? true : false;
   }

   //send the signal
   bool signal() {
      return (pthread_cond_signal(&m_cmCondVar) == 0) ? true : false;
   }

   //broadcast the signal
   bool broadcast() {
      return (pthread_cond_broadcast(&m_cmCondVar) == 0) ? true : false;
   }

private:
   //condition var
   pthread_cond_t m_cmCondVar;
};

#endif
