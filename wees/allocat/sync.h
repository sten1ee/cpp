//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Lightweight mutex implementation for Win32 single processor systems
// To turn on mutex logging facility define macro `ALLOCATOR_MUTEX_LOG'
// Send comments and suggestions to stenly@sirma.bg
// Last modified 14.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef MUTEX_H
#define MUTEX_H

#include <stdio.h>

#define SYNC_ASSERT(M,C)\
  if (!(C)) assert_failed(k, #M, #C, __FILE__, __LINE__);\
  (void*)0

class mutex
{
  volatile int val;
  volatile int key;

  public:

    mutex();
   ~mutex();

    bool  try_acquire(int k);
    void  acquire    (int k);
    void  release    (int k);

    bool  matches(int   k) const { return key == k;      }
    bool  matches(void* k) const { return key == (int)k; }

    bool  try_acquire(void* k) { return try_acquire((int)k); }
    void  acquire    (void* k) { acquire((int)k);            }
    void  release    (void* k) { release((int)k);            }

    void  assert_failed(int k, const char* m, const char* c, const char* f, int l);
    void  dump(FILE* f, int k=0);    
#ifdef  ALLOCATOR_MUTEX_LOG
    void  log(const char* msg, int k=0);
#else
    void  log(const char* , int =0) {;}
#endif
};


inline
mutex::mutex()
  : key(0), val(-1)
  {
    log("created");
  }

inline
mutex::~mutex()
  {
    int k = 0;
    SYNC_ASSERT(~mutex, key ==  0);
    SYNC_ASSERT(~mutex, val == -1);
    log("destroyed");
  }

inline
bool  mutex::try_acquire(int k)
  {
    SYNC_ASSERT(try_acquire, k > 0);
    ++val;
    SYNC_ASSERT(try_acquire, val >= 0);
    if (val == 0) {
      SYNC_ASSERT(try_acquire, key == 0);
      key = k;
      log("try_acquire");
      return true;
    }
    else {
      log("try_acquire failed", k);
      --val;
      return false;
    }
  }


class auto_mutex
{
  mutex& m;

  public:
    auto_mutex(mutex& _m) : m(_m) { m.acquire((int)this); }
   ~auto_mutex()                  { m.release((int)this); }
};

#endif
