#ifndef PG_PREDEFS_H
#define PG_PREDEFS_H

#include <limits.h>
#include "util/sortvec.h"
#define SET sorted_vector
#include "util/setoper.h"

#define CHAR_FIRST 0
#ifndef CHAR_LAST
# define CHAR_LAST 255
#endif
#define CHAR_COUNT ((CHAR_LAST) + 1 - (CHAR_FIRST))

struct int_sortvec_traits : sortvec_traits< int >
{
  static int  compare(int c1, int c2)
    { return c1 < c2 ? -1
           : c1 > c2 ? +1 : 0; }
};

typedef int Char;

typedef sorted_vector< int_sortvec_traits >  CharSet;

const Char EPS_CHAR = INT_MIN;

class State;
class FA;
class RE;

#endif//PG_PREDEFS_H
