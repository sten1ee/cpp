#ifndef UTIL_SORTVECT_H
#define UTIL_SORTVECT_H

#ifndef   UTIL_VECTORT_H
#include "util/vectort.h"
#endif

template <class DATA> class SortVecTraits : public VectorTraits<DATA>
{
  public:
    static  int Compare(DataId x, DataId y)
      {
        return x < y ? -1 : x == y ? 0 : +1;
      }
};


template <class DATA> class SortVecPtrTraits : public VectorPtrTraits<DATA>
{
  public:
    static  int Compare(DataId x, DataId y)
      {
        return *x < *y ? -1 : *x == *y ? 0 : +1;
      }
};

#endif //UTIL_SORTVECT_H
