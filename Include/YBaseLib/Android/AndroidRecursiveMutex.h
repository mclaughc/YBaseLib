#pragma once
#include "YBaseLib/Assert.h"
#include "YBaseLib/Common.h"
#include <pthread.h>

class RecursiveMutex
{
  friend class ConditionVariable;

public:
  RecursiveMutex()
  {
    pthread_mutexattr_t mutexAttributes;
    pthread_mutexattr_init(&mutexAttributes);
    pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_Mutex, &mutexAttributes);
    pthread_mutexattr_destroy(&mutexAttributes);
  }

  ~RecursiveMutex() { pthread_mutex_destroy(&m_Mutex); }

  void Lock() { pthread_mutex_lock(&m_Mutex); }

  bool TryLock() { return (pthread_mutex_trylock(&m_Mutex) == 0); }

  void Unlock() { pthread_mutex_unlock(&m_Mutex); }

private:
  pthread_mutex_t m_Mutex;
};
