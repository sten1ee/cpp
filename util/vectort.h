#ifndef UTIL_VECTORT_H
#define UTIL_VECTORT_H

#include <string.h>
#include <malloc.h>

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif

#ifndef _MSC_VER
# pragma intrisinc memcpy
# pragma intrisinc memmove
#endif

template <class DATA> class vector_traits
{
  public:
    typedef       DATA    Data;
    typedef const DATA&   DataIn;
    typedef const DATA&   DataKey;

    static  DataKey keyof(DataIn data)
      {
        return data;
      }

//------------------------------------------------------------------------------
    static  Data* alloc_data_array(int capacity)
      {
        return  (Data*)malloc(capacity * sizeof(Data));
      }

    static  Data* realloc_data_array(Data* data, int data_count, int new_capacity)
      {
        Data* new_data = (Data*)realloc(data, new_capacity * sizeof(Data));
        UTIL_ASSERT(new_data);
        return new_data;
      }

    static  void  dealloc_data_array(Data* data, int /*capacity*/)
      {
        free(data);
      }

    static  void  destroy_data_array(Data* /*data*/, int /*dataCount*/, bool /*ownsData*/)
      {
      }

//------------------------------------------------------------------------------
    static  void  remove_entries(int n_entries, Data* position, int move_count)
      {
        memmove(position, position + n_entries, move_count * sizeof(Data));
      }

    static  void  insert_entry(int n_entries, Data* position, int move_count)
      {
        memmove(position + n_entries, position, move_count * sizeof(Data));
      }

//------------------------------------------------------------------------------
    static  int   default_capacity()
      {
        return 32;
      }

    enum
      {
        NO_INCREMENT     = -1,
        DOUBLE_INCREMENT =  0,
      };

    static  int   default_capacity_inc()
      {
        return DOUBLE_INCREMENT; // double capacity on increment !
      }

    static  bool  default_owns_data()
      {
        return true;
      }

    static  bool  allow_duplicates()
      {
        return false;
      }
//------------------------------------------------------------------------------      
};


template <class DATA> class vector_traits_ptr : public vector_traits<DATA*>
{
  public:
    typedef typename vector_traits<DATA*>::Data Data;
    static  void  destroy_data_array(Data* data, int data_count, bool owns_data);
//------------------------------------------------------------------------------
};

template <class DATA>
void  vector_traits_ptr<DATA>::destroy_data_array(Data* data, int data_count, bool owns_data)
{
  if (owns_data) {
    for (int i = data_count; 0 < i--; ) {
      delete data[i];
    }
  }
}

#endif //UTIL_VECTORT_H
