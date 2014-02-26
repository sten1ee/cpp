#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include <stddef.h>
#include <stdlib.h>

#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif


template <class DATA>
struct array_traits
{
  typedef       DATA  Data;
  typedef const DATA& DataIn;

  enum { initial_capacity = 32 };

  static ptrdiff_t  new_capacity(ptrdiff_t old_capacity)
    {
      if (old_capacity <= 32)
        return old_capacity + 32;
      if (old_capacity <= 1024)
        return 2 * old_capacity;
      return old_capacity + 1024;
    }
};

template <class DATA>
class array_ref
{
  public:
    typedef array_traits<DATA>           Traits;
    typedef typename Traits::Data        Data;
    typedef typename Traits::DataIn      DataIn;
    typedef          Data*               iterator;
    typedef    const Data*               const_iterator;

  protected:
    Data* data;

    struct rep_t {
      Data* end_storage;
      Data* end_data;
      Data  data[1];
    };

    rep_t*  rep()
      {
        return (rep_t*)((char*)data - offsetof(rep_t, data));
      }

    const rep_t*  rep() const
      {
        return (rep_t*)((char*)data - offsetof(rep_t, data));
      }

    array_ref()
      {
      }

  public:
    array_ref(const array_ref& arr)
      {
        data = arr.data;
      }

    array_ref& operator = (const array_ref& arr)
      {
        data = arr.data;
        return *this;
      }

    ptrdiff_t  size() const
      {
        return end() - begin();
      }

    void  flush()
      {
        rep()->end_data = rep()->data;
      }

    bool   empty() const
      {
        return end() == begin();
      }

    Data& operator [](int idx)
      {
        UTIL_ASSERT(0 <= idx && idx < size());
        return data[idx];
      }

    const Data& operator [](int idx) const
      {
        UTIL_ASSERT(0 <= idx && idx < size());
        return data[idx];
      }

    iterator  begin()
      {
        return data;
      }

    iterator  end()
      {
        return rep()->end_data;
      }

    const_iterator  begin() const
      {
        return data;
      }

    const_iterator  end() const
      {
        return rep()->end_data;
      }
};

template <class DATA>
class array : public array_ref<DATA>
{
  protected:
    static Data*  malloc(int capacity);
    static Data*  realloc(Data* data, ptrdiff_t new_capacity);
    static void   free(Data* data);

    array(const array&) {;}
    void  operator = (const array&) {;}

    ptrdiff_t  capacity() const
      {
        return rep()->end_storage - rep()->data;
      }

  public:
    array()
      {
        data = malloc(Traits::initial_capacity);
      }

    array(int capacity)
      {
        data = malloc(capacity);
      }

    array(Data* src, int sz);

   ~array()
      {
        free(data);
      }

    void  push(DataIn d);
};

template <class DATA>
typename array<DATA>::Data*
array<DATA>::malloc(int capacity)
{
  rep_t* rep = (rep_t*)::malloc(sizeof(rep_t) - sizeof(Data)
                                              + sizeof(Data) * capacity);
  UTIL_ASSERT(rep);
  rep->end_storage = rep->data + capacity;
  rep->end_data    = rep->data;
  return rep->data;
}

template <class DATA>
typename array<DATA>::Data*
array<DATA>::realloc(Data* data, ptrdiff_t new_capacity)
{
  rep_t*    rep  = (rep_t*)((char*)data - offsetof(rep_t, data));
  ptrdiff_t size = rep->end_data - rep->data;
  rep = (rep_t*)::realloc(rep, sizeof(rep_t) - sizeof(Data)
                                             + sizeof(Data) * new_capacity);
  UTIL_ASSERT(rep);
  rep->end_data    = rep->data + size;
  rep->end_storage = rep->data + new_capacity;
  return rep->data;
}

template <class DATA>
void
array<DATA>::free(Data* data)
{
  rep_t* rep = (rep_t*)((char*)data - offsetof(rep_t, data));
  ::free(rep);
}

template <class DATA>
array<DATA>::array(Data* src, int sz)
{
  UTIL_ASSERT(0 <= sz);
  Data* dst = data = malloc(sz);
  rep()->end_data = dst + sz;
  while (0 <= --sz)
    {
      *dst++ = *src++;
    }
}

template <class DATA>
void
array<DATA>::push(DataIn d)
{
  rep_t* rep = this->rep();
  if (rep->end_storage == rep->end_data)
    {
      data = realloc(data, Traits::new_capacity(capacity()));
      rep = this->rep();
    }
  *rep->end_data++ = d;
}

#endif //util/array.h
