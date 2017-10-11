#ifndef UTIL_SORTVEC_H
#define UTIL_SORTVEC_H

#ifndef   UTIL_VECTOR_H
#include "util/vector.h"
#endif

/**
  The sorted_vector is a vector whose elements are sorted in increasing order.
  This restriction gives us the ability to make a key-based element lookup
  in at most O(log n) where n is the size of the sorted_vector.
*/
template <class TRAITS> class sorted_vector : protected vector<TRAITS>
{
    typedef vector<TRAITS>  parent;

  public:
    typedef typename parent::Traits          Traits;
    typedef typename parent::Data            Data;
    typedef typename parent::DataIn          DataIn;
    typedef typename parent::iterator        iterator;
    typedef typename parent::const_iterator  const_iterator;
    typedef typename parent::copy_tag        copy_tag;
    typedef typename parent::alias_tag       alias_tag;

    typedef typename Traits::DataKey DataKey;
    
    using parent::COPY;
    using parent::ALIAS;

    using parent::size;
    using parent::empty;
    using parent::owns_data;
    using parent::ensure_insert;
    using parent::capacity;
    using parent::set_capacity;
    using parent::trim_to_size;
    using parent::erase;
    using parent::erase_front;
    using parent::erase_back;
    using parent::pop_back;
    using parent::pop;
    using parent::top;
    using parent::begin;
    using parent::end;
    using parent::front;
    using parent::back;
    using parent::flush;
    using parent::reserve;
    using parent::resize;
    using parent::operator [];

  protected:
    using parent::push_back;
    using parent::header;
    using parent::data;

  public:
    sorted_vector()
      : parent()
      {}

    sorted_vector(bool owns_data)
      : parent(owns_data)
      {}

    sorted_vector(int initial_capacity, int capacity_inc)
      : parent(initial_capacity, capacity_inc)
      {}

    sorted_vector(const sorted_vector& sv)
      : parent(sv)
      {}

    sorted_vector(const sorted_vector& sv, copy_tag)
      : parent(sv, COPY)
      {}

    sorted_vector(const sorted_vector& sv, alias_tag)
      : parent(sv, ALIAS)
      {}

    void  alias(const sorted_vector& sv) { parent::alias(sv); }
    void  copy(const sorted_vector& sv)  { parent::copy(sv);  }
    void  swap(sorted_vector& sv)        { parent::swap(sv);  }
#ifdef  OPTION_HARD_COPY
    void  assign(const sorted_vector& v)  { copy(v);  }
#else // make only alias upon assignment:
    void  assign(const sorted_vector& v)  { alias(v); }
#endif

    sorted_vector&  operator = (const sorted_vector& sv)
      {
        assign(sv);
        return  *this;
      }

    const parent& to_vector() const { return *this; }

    class  finger : public iterator
    {
      protected:
        friend class sorted_vector<Traits>;

        finger(vector_header<Data>* header, int index)
          : iterator(header, index)
        {}

        using iterator::index;
        using iterator::header;

      public:
                  finger() {}
                  finger(const iterator& i) : iterator(i) {}
                  operator bool () const { return  0 <= index; }
        bool      operator !    () const { return  index < 0;  }
        Data&     operator *    () const
          {
            UTIL_ASSERT(0 <= index);
            UTIL_ASSERT(index < header->size);
            return header->data[index];
          }
    };

    finger  find(DataKey key);

    void  insert(finger& fi, DataIn elem)
      {
        UTIL_ASSERT(fi.header == header());
        int index = -1 - fi.index;
        if (index < 0) {
          UTIL_ASSERT(Traits::allow_duplicates());
          index = -1 - index;
        }
        UTIL_ASSERT(index <= size());
        UTIL_DEBUGIN (
          if (index != 0) {
            int cmp = Traits::compare(Traits::keyof((*this)[index-1]),
                                      Traits::keyof(elem));
            UTIL_ASSERT(cmp < 0 || cmp == 0 && Traits::allow_duplicates());
          }
          if (index != size()) {
            int cmp = Traits::compare(Traits::keyof(elem),
                                      Traits::keyof((*this)[index]));
            UTIL_ASSERT(cmp < 0 || cmp == 0 && Traits::allow_duplicates());
          }
        )
        fi.index = index;
        parent::insert(fi, elem);
      }

    // assumes data is greater than any element in the sorted vector:
    void  insert_back(DataIn elem)
      {
        UTIL_DEBUGIN(
          if (!empty()) {
            int cmp = Traits::compare(Traits::keyof(data()[size()-1]),
                                      Traits::keyof(elem));
            UTIL_ASSERT(cmp < 0 || cmp == 0 && Traits::allow_duplicates());
          }
        )
        push_back(elem);
      }

    // assumes data is not equal to any element in the sorted vector
    // or duplicates are allowed.
    void  insert(DataIn data)
      {
        finger  fi = find(Traits::keyof(data));
        UTIL_ASSERT(!fi || Traits::allow_duplicates());
        insert(fi, data);
      }

    // if  data is not equal to any element already stored
    // inserts it & returns true; otherwise returns false.
    bool  insert_unique(DataIn data)
      {
        finger  fi = find(Traits::keyof(data));
        if (fi)
          return false;
        insert(fi, data);
        return true;
      }
};


template <class TRAITS>
typename sorted_vector<TRAITS>::finger  sorted_vector<TRAITS>::find(DataKey key)
{
  int  low = 0, high = size(), mid;
  while (low < high) {
    mid = (low + high)/2;
    int diff = Traits::compare(key, Traits::keyof(data()[mid]));
    if (diff < 0) {
      high = mid;
    }
    else if (diff > 0) {
      low = mid + 1;
    }
    else {
      return finger(header(), mid); // found
    }
  }
  return finger(header(), -1 - low); // not found
}

#ifndef UTIL_SORTVECT_H
#include "util/sortvect.h"
#endif

#ifdef  SORTED_VECTOR_SETOPER
#  ifdef  SET
#    ifdef  SET_OLD_VALUE
#      error this really sux ...
#    endif
#    define SET_OLD_VALUE SET
#    undef  SET
#  endif

#  define SET sorted_vector
#    include "util/setoper.h"
#  undef  SET

#  ifdef  SET_OLD_VALUE
#    define SET SET_OLD_VALUE
#    undef  SET_OLD_VALUE
#  endif
#endif
  
#endif //UTIL_SORTVEC_H
