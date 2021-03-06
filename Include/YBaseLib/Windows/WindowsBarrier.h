#pragma once
#include "YBaseLib/Atomic.h"
#include "YBaseLib/Event.h"

#ifdef Y_PLATFORM_WINDOWS
#include "YBaseLib/Windows/WindowsHeaders.h"
#endif

class Barrier
{
public:
  Barrier(uint32 threadCount);
  ~Barrier();

  const uint32 GetThreadCount() const { return m_threadCount; }
  void SetThreadCount(uint32 threadCount);

  void Wait();

private:
  uint32 m_threadCount;
  Y_ATOMIC_DECL uint32 m_enteredThreadCount;
  Y_ATOMIC_DECL uint32 m_exitedThreadCount;
  HANDLE m_hEnterSemaphore;
  HANDLE m_hExitSemaphore;
};
