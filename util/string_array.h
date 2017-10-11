#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include "util/string.h"

template <class DATA>
class array : private String<DATA>
{
    typedef String<DATA>  parent;
    typedef DATA          Data;

    using parent::length;
    using parent::append;

  public:
    array()                           : parent()        {}
    array(const array& a)             : parent(a)       {}
    array(const Data* da, int len) : parent(da, len) {}

    array& operator = (const array& a) { return (array&)parent::operator= (a); }

//------------------------------------------------------------------------------
    typedef const Data* const_iterator;
    typedef       Data*       iterator;

    const_iterator  begin() const { return rep_chr; }
          iterator  begin()       { return rep_chr; }

    const_iterator  end  () const { return rep_chr + size(); }
          iterator  end  ()       { return rep_chr + size(); }


    int   size  () const { return length(); }
    bool  empty () const { return length() == 0; }

    void  push(Data d) { append(d); }

    const Data&  operator [](int pos) const
      {
        UTIL_ASSERT(pos < size());
        return rep_chr[pos];
      }

    Data&  operator [](int pos)
      {
        UTIL_ASSERT(pos < size());
        return rep_chr[pos];
      }
};

#endif //UTIL_STRING_H
