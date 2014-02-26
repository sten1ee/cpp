#ifndef UTIL_SORTVECT_H
#define UTIL_SORTVECT_H

#ifndef   UTIL_VECTORT_H
#include "util/vectort.h"
#endif

template <class DATA> class sortvec_traits : public vector_traits<DATA>
{
  public:
//  static  int compare(DataKey x, DataKey y)
//    {
//      return x < y ? -1 : x == y ? 0 : +1;
//    }
};


template <class DATA> class sortvec_traits_ptr : public vector_traits_ptr<DATA>
{
  public:
//  static  int compare(DataKey x, DataKey y)
//    {
//      return *x < *y ? -1 : *x == *y ? 0 : +1;
//    }
};

#endif //UTIL_SORTVECT_H
