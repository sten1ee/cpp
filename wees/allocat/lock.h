//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Lightweight lock implementation for Win32 single processor systems
// Send comments and suggestions to stenly@sirma.bg
// Last modified 19.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef LOCK_H
#define LOCK_H

typedef volatile int lock_t;

#define LOCK_INIT_VALUE (-1)

inline
void  create_lock(lock_t& lock)
  {
    lock = -1;
  }

inline
void  destroy_lock(lock_t& /*lock*/)
  {
    // ASSERT(lock == -1);
  }

inline
bool  try_acquire_lock(lock_t& lock)
  {
    ++lock;
    if (lock == 0) {
      return true;
    }
    else {
      --lock;
      return false;
    }
  }

#ifndef NO_INLINE_LOOPS // BC++ doesn't inline functions containing loops !
inline
void  acquire_lock(lock_t& lock)
  {
    do {
      ++lock;
      if (lock == 0) {
        return;
      }
      --lock;
      Sleep(0);
    } while (true);
  }
#else
#define acquire_lock(LOCK)    \
  {                           \
    lock_t* __lock = &(LOCK); \
    do {                      \
      ++*__lock;              \
      if (*__lock == 0)       \
        break;                \
      --*__lock;              \
      Sleep(0);               \
    } while (true);           \
  }
#endif

inline
void  release_lock(lock_t& lock)
  {
    --lock;
  }

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class auto_lock
{
  protected:
    lock_t& lock;

  public:
    auto_lock(lock_t& l) : lock(l) { acquire_lock(lock); }
   ~auto_lock()                    { release_lock(lock); }
};
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#endif
