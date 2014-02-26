//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Test code (see allocat.h & allocat.cpp)
// Send comments and suggestions to stenly@sirma.bg
// Last modified 19.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <windows.h>
#include <stdio.h>
#include <sys/timeb.h>

#include "malloc.h"
#include "allocat.h"

#ifdef ALLOCATOR_DIAG
# include "diag.h"
#endif


struct ALLOCATOR_VOID
{
  static int          int_var;
  static void*        allocate(size_t ) { return &int_var; }
  static void         deallocate(void*) { ; }
  static const char*  name() { return "void/void"; }
  static void         print_params(FILE*) {;}
};
int ALLOCATOR_VOID::int_var;

struct ALLOCATOR_NEW
{
  static void* allocate(size_t  sz) { return new char[sz]; }
  static void  deallocate(void* vp) { delete[] vp; }
  static const char*  name() { return "new/delete"; }
  static void  print_params(FILE*) {;}
};

struct ALLOCATOR_MALLOC
{
  static void* allocate(size_t  sz) { return ::malloc(sz); }
  static void  deallocate(void* vp) { ::free(vp); }
  static const char*  name() { return "malloc/free"; }
  static void  print_params(FILE*) {;}
};

struct ALLOCATOR_ALLOC
{
  static void* allocate(size_t  sz) { return ::allocate(sz); }
  static void  deallocate(void* vp) { ::deallocate(vp); }
  static const char*  name() { return "allocate/deallocate"; }
  static void  print_params(FILE* f)
    {
#if defined(ALLOCATOR_DIAG)
      diag::print_allocator_params(f);
#endif
    }
};


double  operator - (const struct timeb& t1, const struct timeb& t2)
{
  long mls = (t1.time - t2.time) * 1000 + (t1.millitm - t2.millitm);
  return (mls / 1000) + (mls % 1000) / 1000.0;
}

#ifdef ALLOCATOR_DIAG
void  show_stats(int step)
{
  using namespace diag;
  printf("step = %10d\n", step);
  printf("\tcell  alocations\t%10d /%10d (%10d)\n", (int)c_allocate.pos, (int)c_allocate.neg, (int)c_allocate.max_diff);
  printf("\tblock alocations\t%10d /%10d (%10d)\n", (int)c_allocate_block.pos, (int)c_allocate_block.neg, (int)c_allocate_block.max_diff);
}
#endif

const p = 10001, q = 703;

inline
unsigned random(unsigned& ra, unsigned NN)
  {
    ra = ra*p + q;
    return ra % NN;
  }

inline
int min(int p, int q)
  {
    return p < q ? p : q;
  }

  
struct arg_t
{
  HANDLE  semaphore;
  HANDLE  thread_death_event;
  int     n_threads;
  int     n_finished;
  int     n_steps;
  int     n_nodes;
  size_t  sz_fixed;
  bool    b_consec;
  bool    b_verbose;
};


class base_tester
{
  public:
    virtual ~base_tester() {;}

    virtual HANDLE  create_thread(arg_t* arg)=0;
    virtual void    print_test_params(const arg_t& arg)=0;

    static  void    show_progress(const arg_t* arg)
      {
        if (arg->b_verbose)
          printf("\r%s %d/%d", (arg->b_consec ? "Laps finished" : "Progress"),
                               (n_steps_total / arg->n_steps), arg->n_threads);
      }

    static volatile unsigned  n_steps_total;
};

volatile unsigned  base_tester::n_steps_total;


template <class ALLOCATOR>
class allocator_tester : public base_tester
{
  public:
    virtual HANDLE      create_thread(arg_t* arg);
    virtual void        print_test_params(const arg_t& arg);
    static void         run_test(const arg_t* arg);
    static DWORD WINAPI thread_starter(LPVOID vp_arg);
};


template <class ALLOCATOR>
void  allocator_tester<ALLOCATOR>::run_test(const arg_t* arg)
{
  const int    nsteps   = arg->n_steps;
  const int    NN       = arg->n_nodes;
  const size_t sz_fixed = arg->sz_fixed;
  unsigned     ra = 90007; // random accumulator
  void**const node = new void*[NN];
  int i; // declare `i' here so VC++ doesn't mess up.
  for (i=0; i < NN; i++) {
    node[i] = 0;
  }
#ifdef  TRACE_RUN
  show_stats(0);
#endif
  timeb  tb1, tb2, tb3;
  ftime(&tb1);
  int max_alloc    = 100;
  int max_dealloc  = 100;
  for (int step=1; step <= nsteps; step++) {
    if (++n_steps_total % nsteps == 0) {
      show_progress(arg);
    }
    i = random(ra, NN);
    if (node[i] != 0) {
      for (int j=min(max_alloc, NN - i); 0 < j; i++, j--) {
        if (node[i] == 0)
          break;
        ALLOCATOR::deallocate(node[i]);
        node[i] = 0;
      }
    }
    else {
      size_t sz = (sz_fixed ? sz_fixed : i % 256 + 1);
      for (int j=min(max_dealloc, NN - i); 0 < j; i++, j--) {
        if (node[i] != 0)
          break;
        node[i] = ALLOCATOR::allocate(sz);
      }
    }
#ifdef  TRACE_RUN
    if (step % (nsteps / 10) == 0) {
      show_stats(step);
    }
#endif
  }
  ftime(&tb2);
  for (i=0; i < NN; i++)
    if (node[i])
      ALLOCATOR::deallocate(node[i]);
  ftime(&tb3);
#ifdef  TRACE_RUN
  printf("tests done in %f\n", (tb2 - tb1));
  printf("final done in %f\n", (tb3 - tb2));
  printf("total done in %f\n", (tb3 - tb1));
#endif

  delete[] node;
}


template <class ALLOCATOR>
DWORD WINAPI allocator_tester<ALLOCATOR>::thread_starter(LPVOID vp_arg)
{
  arg_t* arg = (arg_t*)vp_arg;
  run_test(arg);
  WaitForSingleObject(arg->semaphore, INFINITE);
  arg->n_finished++;
  SetEvent(arg->thread_death_event);
  ReleaseSemaphore(arg->semaphore, 1, 0);
  return 0;
}


template <class ALLOCATOR>
HANDLE  allocator_tester<ALLOCATOR>::create_thread(arg_t* arg)
{
  if (arg->b_consec) {
    static HANDLE handle;
    if (handle == 0)
      handle = GetCurrentThread();
    thread_starter(arg);
    return handle;
  }
  else {
    DWORD thread_id;
    return CreateThread(
      0,              // pointer to thread security attributes
      0,              // initial thread stack size, in bytes
      thread_starter, // pointer to thread function
      (LPVOID)arg,	  // argument for new thread
      0,	            // creation flags
      &thread_id      // pointer to returned thread identifier
     );
  }
}


template <class ALLOCATOR>
void  allocator_tester<ALLOCATOR>::print_test_params(const arg_t& arg)
{
  printf("[test]\n");
  printf("\tusing `%s'\n", ALLOCATOR::name());
  printf("\tn_threads=%d, n_steps=%d, n_nodes=%d, b_consec=%s, sz_fixed = %d;\n",
         arg.n_threads, arg.n_steps, arg.n_nodes,
         (arg.b_consec ? "yes" : "no"), arg.sz_fixed);
  ALLOCATOR::print_params(stdout);
}


void print_help()
{
  printf("Recognized cmd line args:\n"
         "\tt<N>\trun <N> threads/laps\n"
         "\ti<N>\teach thread/lap makes <N> iterations\n"
         "\tn<N>\teach thread/lap works on array of length <N>\n"
         "\ts<N>\tallocate only objects of fixed size <N>\n"
         "\tl   \trun laps instead of threads (i.e. single threaded)\n"
         "\tv   \tverbose - show progress\n"
         "\ta<A>\tuse allocator <A> (m=malloc/free, n = new/delete,\n"
         "\t    \t                   a=allocate/deallocate, v = nop/nop)\n"
         "\th   \tprint this help and exit");
}


int  main(int nn, char** aa)
{
  int    n_threads = 16;
  int    n_steps   = 10000; // 1000000 for single thread
  int    n_nodes   = 10000; // 1000000 for single thread
  size_t sz_fixed  = 0;     // allocate fixed size blocks ?
  bool   b_consec  = false; // consecutive or parallel execution
  bool   b_verbose = false; // print progress on stdout
  char   c_alloc   = 'a';   // allocator to use

  arg_t arg;

  for (int n=1; n < nn; n++) {
    switch (aa[n][0]) {
      case 't': n_threads = atoi(aa[n] + 1); break;
      case 'i': n_steps   = atoi(aa[n] + 1); break;
      case 'n': n_nodes   = atoi(aa[n] + 1); break;
      case 's': sz_fixed  = atoi(aa[n] + 1); break;
      case 'l': b_consec  = true;            break;
      case 'v': b_verbose = true;            break;
      case 'a': c_alloc   = aa[n][1];        break;
      case 'h': print_help(); return 0;
      default:
        printf("Unrecognized cmd line arg `%s' !\n", aa[n]);
        print_help();
        return -1;
    }
  }

  arg.n_threads   = n_threads;
  arg.n_finished  = 0;
  arg.n_steps     = n_steps;
  arg.n_nodes     = n_nodes;
  arg.sz_fixed    = sz_fixed;
  arg.b_consec    = b_consec;
  arg.b_verbose   = b_verbose;

#if !defined(ALLOCATOR_THREADSAFE)
  if (c_alloc == 'a' && !b_consec) {
    printf("This is the single threaded version of allocator\n"
           "To use with multiple threads build allocator with `ALLOCATOR_THREADSAFE' defined");
    return -1;
  }
#endif

  if (0 == (arg.semaphore = CreateSemaphore(0, 1, 1,	0))) {
    printf("Unable to create semaphore object !");
    return -1;
  }
  if (0 == (arg.thread_death_event = CreateEvent(0, TRUE, TRUE, 0))) {
    printf("Unable to create event object !");
    return -1;
  }

  base_tester* tester;

  switch (c_alloc) {
    case 'm': tester = new allocator_tester<ALLOCATOR_MALLOC>;
              break;
    case 'n': tester = new allocator_tester<ALLOCATOR_NEW>;
              break;
    case 'a': tester = new allocator_tester<ALLOCATOR_ALLOC>;
              break;
    case 'v': tester = new allocator_tester<ALLOCATOR_VOID>;
              break;
    default : printf("Unknown allocator type `%c' !", c_alloc);
              return -1;
  }

  tester->print_test_params(arg);
  
  timeb  tb1, tb2;
  ftime(&tb1);

  for (int i = 0; i < n_threads; i++) {
    if (!tester->create_thread(&arg)) {
      printf("Failed to start thread %2d !\n", i+1);
      arg.n_threads--;
    }
  }

  do {
    WaitForSingleObject(arg.thread_death_event, INFINITE);
    ResetEvent(arg.thread_death_event);
  } while (arg.n_finished < arg.n_threads);

  ftime(&tb2);
  if (b_verbose) { // Wipe the last "Finished lap N"
    printf("\r");
  }
#if defined(ALLOCATOR_DIAG)
  if (c_alloc == 'a') {
    diag::print_allocator_stats(stdout);
    tester->print_test_params(arg);
  }
#endif
  printf("test done in %f\n", (tb2 - tb1));

  delete tester;
  CloseHandle(arg.semaphore);
  CloseHandle(arg.thread_death_event);

  return 0;
}
