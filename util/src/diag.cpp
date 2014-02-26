//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Diag code (see allocat.h & allocat.cpp)
// Send comments and suggestions to stenly@sirma.bg
// Last modified 14.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef ALLOCATOR_DIAG

#include "util/diag.h"

namespace diag {

enum { SZ_MAX = 256 };

counter c_allocate;
counter c_allocate_block;
counter n_allocated[SZ_MAX];


void   print_allocator_stats(FILE* f)
{
  fprintf(f, "================================================================\n");
  for (int i=0; i < SZ_MAX; i++) {
    if (n_allocated[i].max_diff != 0) {
      fprintf(f, "sz =%4d, pos = %9d, neg = %9d, max_diff = %9d\n",
        i,
        n_allocated[i].pos,
        n_allocated[i].neg,
        n_allocated[i].max_diff
        );
    }
  }
  fprintf(f, "================================================================\n");
  fprintf(f, "total cell  alocations\t%10d /%10d (%10d)\n", (int)c_allocate.pos, (int)c_allocate.neg, (int)c_allocate.max_diff);
  fprintf(f, "total block alocations\t%10d /%10d (%10d)\n", (int)c_allocate_block.pos, (int)c_allocate_block.neg, (int)c_allocate_block.max_diff);
  fprintf(f, "================================================================\n");
}

} //namespace diag
#endif
