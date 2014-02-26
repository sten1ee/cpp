#ifndef UTIL_ALLOC_H
#define UTIL_ALLOC_H

#if defined(ALLOCATOR_DIAG) && !defined(ENABLE_FAST_ALLOC)
# define ENABLE_FAST_ALLOC
#endif

#ifdef ENABLE_FAST_ALLOC
# include "util/allocat.h"

# define FAST_ALLOC \
  void* operator new    (size_t sz) { return allocate(sz); }\
  void  operator delete (void*  vp) { deallocate(vp); }
#else
# define FAST_ALLOC
#endif

#endif
