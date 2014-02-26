//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Lightweight mutex implementation for Win32 single processor systems
// To turn on mutex logging facility define macro `ALLOCATOR_MUTEX_LOG'
// Send comments and suggestions to stenly@sirma.bg
// Last modified 14.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <windows.h>
#include "sync.h"
#include "lock.h"

void  mutex::acquire(int k)
{
  SYNC_ASSERT(acquire, k > 0);
  do {
    ++val;
    SYNC_ASSERT(acquire, val >= 0);
    if (val == 0) {
      SYNC_ASSERT(acquire, key == 0);
      key = k;
      log("acquire");
      break;
    }
    log("acquire failed; Sleep", k);
    --val;
    Sleep(0);
  } while (true);
}


void  mutex::release(int k)
{
  SYNC_ASSERT(release, k > 0);
  SYNC_ASSERT(release, key == k);
  SYNC_ASSERT(release, val >= 0);
  log("release");
  key = 0;
  --val;
}

static lock_t log_lock = LOCK_INIT_VALUE;

#if defined(ALLOCATOR_MUTEX_LOG)

static FILE*  logf      = fopen("mutex.log", "w");
static int    line      = 0;

void  mutex::log(const char* msg, int k)
{
  acquire_lock(log_lock);

  if (line++ % 4096 == 0) { // truncate file
    if (logf != 0)
      fclose(logf);
    logf = fopen("mutex.log", "w");
  }

  fprintf(logf, "%d ", line);
  dump(logf, k);
  fprintf(logf, "; %s\n", msg);

  release_lock(log_lock);
}

#endif
void  mutex::dump(FILE* f, int k)
{
  fprintf(f, "mutex[%p] key=%p, val=%2d", (void*)this, (void*)key, (int)val);
  if (k)
    fprintf(f, ", new_key=%p", (void*)k);
}


void  mutex::assert_failed(int k, const char* m, const char* c, const char* f, int l)
{
  acquire_lock(log_lock);

  fprintf(stderr, "\nSYNC_ASSERT (%s) failed in mutex::%s, file %s, line %d\n",
          c, m, f, l);
  dump(stderr, k);

#if defined(ALLOCATOR_MUTEX_LOG)
  fprintf(logf, "%d ", line);
  dump(logf, k);
  fprintf(stderr, "\nRefer to file `mutex.log' for details");
#else
  fprintf(stderr, "%s",
"\nCompile with `ALLOCATOR_MUTEX_LOG' defined to get details in file `mutex.log'");
#endif

  release_lock(log_lock);
  exit(1);
}

