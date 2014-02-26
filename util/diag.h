//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Diag code (see allocat.h & allocat.cpp)
// Send comments and suggestions to stenly@sirma.bg
// Last modified 14.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef UTIL_DIAG_H
#define UTIL_DIAG_H

#ifdef ALLOCATOR_DIAG

#include <stdio.h>

namespace diag {

struct counter
{
  int pos;
  int neg;
  int max_diff;

  counter() { pos = neg = max_diff = 0; }

  void operator ++ () { if (++pos - neg > max_diff) max_diff = pos - neg; }
  void operator -- () { ++neg; }
};

extern counter c_allocate;
extern counter c_allocate_block;
extern counter n_allocated[];

void  print_allocator_params(FILE*);
void  print_allocator_stats (FILE*);

} //namespace diag

#endif//ALLOCATOR_DIAG

#endif//UTIL_DIAG_H
