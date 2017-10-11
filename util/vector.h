#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <stddef.h>

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif
#include "util/vectort.h"




template  <class DATA>
struct  vector_header
{
    DATA*   data;
    int     size;
    int     capacity;
    int     capacity_inc;
    int     nref;
    bool    owns_data;
};


template <class TRAITS> class vector
{
  public:
    typedef TRAITS   Traits;
    typedef typename Traits::Data        Data;
    typedef typename Traits::DataIn      DataIn;

    class const_iterator {
      friend class vector<Traits>;
      protected:
        vector_header<Data>*  header;
        int                   index;

        const_iterator(vector_header<Data>* _header, int _index)
          {
            header = _header;
            index  = _index;
          }

      public:
        const_iterator()
          {
            header =  0;
            index  = -1;
          }

        const_iterator(const const_iterator& i)
          {
            header = i.header;
            index  = i.index;
          }

        const_iterator& operator = (const const_iterator& i)
          {
            header = i.header;
            index  = i.index;
            return *this;
          }

        bool  operator == (const const_iterator& i) const
          {
            UTIL_ASSERT(header == i.header);
            return index == i.index;
          }

        bool  operator != (const const_iterator& i) const
          {
            UTIL_ASSERT(header == i.header);
            return index != i.index;
          }

        const Data& operator * () const
          {
            UTIL_ASSERT((unsigned)index < (unsigned)header->size);
            return header->data[index];
          }

        int operator - (const const_iterator& i) const
          {
            UTIL_ASSERT(header == i.header);
            return index - i.index;
          }

        const_iterator operator + (int n) const
          {
            UTIL_ASSERT((unsigned)(index + n) < (unsigned)header->size);
            return const_iterator(header, index + n);
          }

        const_iterator operator - (int n) const
          {
            UTIL_ASSERT((unsigned)(index - n) < (unsigned)header->size);
            return const_iterator(header, index - n);
          }

        const_iterator& operator ++ ()
          {
            UTIL_ASSERT(index < header->size);
            ++index;
            return *this;
          }
        const_iterator& operator -- ()
          {
            UTIL_ASSERT(0 < index);
            --index;
            return *this;
          }

        const_iterator  operator ++ (int)
          {
            const_iterator  res = *this;
            ++*this;
            return res;
          }

        const_iterator  operator -- (int)
          {
            const_iterator  res = *this;
            --*this;
            return res;
          }
    };

    class iterator : public const_iterator {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Note: Accessing the inherited field `header' causes a compile time error
//       with some versions of GCC & DJGPP (a compiler bug I believe).
//       If this is the case, defining the macro `WA_VVIHA' (work-around
//       verbose-vector-iterator-header-access) should solve the problem.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef WA_VVIHA
#  define header const_iterator::header
#endif

      friend class vector<Traits>;
      protected:
        iterator(vector_header<Data>* _header, int _index)
            : const_iterator(_header, _index)
          {}

        using const_iterator::header;
        using const_iterator::index;

      public:
        iterator()
          {}

        iterator(const iterator& i)
          : const_iterator(i)
          {}

        iterator& operator = (const iterator& i)
          {
            header = i.header;
            index  = i.index;
            return *this;
          }

        Data& operator * () const
          {
            UTIL_ASSERT((unsigned)index < (unsigned)header->size);
            return header->data[index];
          }

        iterator operator + (int n) const
          {
            UTIL_ASSERT((unsigned)(index + n) < (unsigned)header->size);
            return iterator(header, index + n);
          }

        iterator operator - (int n) const
          {
            UTIL_ASSERT((unsigned)(index - n) < (unsigned)header->size);
            return iterator(header, index - n);
          }

        iterator& operator ++ ()
          {
            UTIL_ASSERT(index < header->size);
            ++index;
            return *this;
          }

        iterator& operator -- ()
          {
            UTIL_ASSERT(0 < index);
            --index;
            return *this;
          }

        iterator  operator ++ (int)
          {
            iterator  res = *this;
            ++*this;
            return res;
          }

        iterator  operator -- (int)
          {
            iterator  res = *this;
            --*this;
            return res;
          }
#ifdef WA_VVIHA
#  undef header
#endif
    };

  private:
    typedef vector_header<Data> header_type;

    header_type* _header;

    void    cons_header(int capacity, int capacity_inc, bool owns_data);
    void    join(const vector& v) { (_header = v._header)->nref++; }
    void    drop();

    Data*&  _data        ()       { return _header->data;         }
    int&    _size        ()       { return _header->size;         }
    int&    _capacity    ()       { return _header->capacity;     }
    int&    _capacity_inc()       { return _header->capacity_inc; }
    bool&   _owns_data   ()       { return _header->owns_data;    }

  protected:
    header_type*  header() const { return _header; }
    Data*         data  () const { return _header->data; }

  public:
    enum  copy_tag  { COPY };
    enum  alias_tag { ALIAS };

    void  copy (const vector& v);
    void  alias(const vector& v)
      {
        if (_header != v._header) {
          drop();
          join(v);
        }
      }

    void  swap(vector& v)
      {
        header_type* tmp = _header;
        _header = v._header;
        v._header = tmp;
      }

#ifdef  OPTION_HARD_COPY
    void  assign(const vector& v)  { copy(v);  }
#else // make only alias upon assignment:
    void  assign(const vector& v)  { alias(v); }
#endif

    static  bool default_owns_data()   { return Traits::default_owns_data(); }
    static  int  default_capacity()    { return Traits::default_capacity();  }
    static  int  default_capacity_inc(){ return Traits::default_capacity_inc();}

    vector()
      {
        cons_header(default_capacity(), default_capacity_inc(),
                    default_owns_data());
      }

    vector(bool owns_data)
      {
        cons_header(default_capacity(), default_capacity_inc(), owns_data);
      }

    vector(int capacity, int capacity_inc)
      {
        cons_header(capacity, capacity_inc, default_owns_data());
      }

    vector(const vector& v, copy_tag)
      {
        cons_header(v.size(), v.capacity_inc(), false);
        insert(begin(), v.begin(), v.end());
      }

    vector(const vector& v, alias_tag)
      {
        join(v);
      }

    vector(const vector& v)
      {
#ifdef  OPTION_HARD_COPY
        cons_header(v.size(), v.capacity_inc(), false);
        insert(v.begin(), v.end());
#else
        join(v);
#endif
      }

   ~vector()
      {
        drop();
      }

    vector&  operator = (const vector& v)
      {
        assign(v);
        return  *this;
      }

    int   size()     const { return _header->size;     }
    int   capacity() const { return _header->capacity; }
    bool  empty()    const { return _header->size == 0;}
    bool  owns_data()const { return _header->owns_data;}


    void  owns_data(bool o){ _header->owns_data = o; }
    void  reserve(int n)   { if (capacity() < n) set_capacity(n); }
    void  resize (int n)   { reserve(n); _size() = n; }
    void  resize (int n, DataIn value);
    void  trim_to_size()   { set_capacity(size()); }

    int   capacity_inc()    const { return _header->capacity_inc; }
    void  set_capacity_inc(int ci){ _header->capacity_inc = ci;   }

    void  ensure_insert();
    void  set_capacity(int new_capacity);

    const_iterator  begin() const { return const_iterator(header(), 0);      }
    const_iterator  end()   const { return const_iterator(header(), size()); }

          iterator  begin()       { return iterator(header(), 0);      }
          iterator  end()         { return iterator(header(), size()); }

    const Data& operator [] (int index) const
      {
        UTIL_ASSERT((unsigned)index < (unsigned)size());
        return data()[index];
      }

    Data& operator [] (int index)
      {
        UTIL_ASSERT((unsigned)index < (unsigned)size());
        return data()[index];
      }

    const Data& front() const
      {
        UTIL_ASSERT(!empty());
        return data()[0];
      }

    Data& front()
      {
        UTIL_ASSERT(!empty());
        return data()[0];
      }

    const Data& back() const
      {
        UTIL_ASSERT(!empty());
        return data()[size()-1];
      }

    Data& back()
      {
        UTIL_ASSERT(!empty());
        return data()[size()-1];
      }

    void  erase(iterator pos)
      {
        UTIL_ASSERT(header() == pos.header);
        UTIL_ASSERT((unsigned)pos.index < (unsigned)size());
        Traits::remove_entries(1, data() + pos.index, --_size() - pos.index);
      }

    void  erase_front()
      {
        UTIL_ASSERT(!empty());
        erase(begin());
      }

    void  erase_back()
      {
        UTIL_ASSERT(!empty());
        erase(--end());
      }

    void  erase(iterator begin, iterator end)
      {
        UTIL_ASSERT(header() == begin.header);
        UTIL_ASSERT(header() == end.header);
        UTIL_ASSERT(0 <= begin.index);
        UTIL_ASSERT(begin.index <= end.index);
        UTIL_ASSERT(end.index <= size());
        Traits::remove_entries(end.index - begin.index, // # entries to remove
                               data() + begin.index,    // from position
                               size() - end.index);     // # ent. to shift down
        _size() -= end.index - begin.index;
      }

    void insert(iterator pos, DataIn elem)
      {
        UTIL_ASSERT(header() == pos.header);
        UTIL_ASSERT((unsigned)pos.index <= (unsigned)size());
        ensure_insert();
        Traits::insert_entry(1, data() + pos.index, _size()++ - pos.index);
        data()[pos.index] = elem;
      }

    void insert(iterator position, const_iterator begin, const_iterator end);

    void push_back(DataIn elem)
      {
        ensure_insert();
        data()[_size()++] = elem;
      }

    void pop_back()
      {
        UTIL_ASSERT(!empty());
        Traits::remove_entries(1, data() + --_size(), 0);
      }

    void  push(DataIn elem)
      {
        ensure_insert();
        data()[_size()++] = elem;
      }

    Data  pop()
      {
        UTIL_ASSERT(!empty());
        Data  temp = data()[--_size()];
        Traits::remove_entries(1, data() + size(), 0);
        return temp;
      }

    const Data& top() const
      {
        UTIL_ASSERT(!empty());
        return data()[size()-1];
      }

    Data& top()
      {
        UTIL_ASSERT(!empty());
        return data()[size()-1];
      }

    void flush(bool owns_data)
      {
        Traits::destroy_data_array(data(), size(), owns_data);
        _size() = 0;
      }

    void flush()
      {
        flush(owns_data());
      }
};

template <class TRAITS>
void  vector<TRAITS>::drop()
{
  if (_header->nref == 0) {
    Traits::destroy_data_array(data(), size(), owns_data());
    Traits::dealloc_data_array(data(), capacity());
    delete _header;
  }
  else {
    _header->nref--;
  }
  _header = 0;
}

template <class TRAITS>
void  vector<TRAITS>::cons_header(int capacity, int capacity_inc,
                                  bool owns_data)
{
  UTIL_ASSERT(0 <= capacity && -1 <= capacity_inc);
  _header = new header_type;
  _header->nref = 0;
  _size() = 0;
  _data() = Traits::alloc_data_array(capacity);
  UTIL_ASSERT(data() != 0 || capacity == 0);
  _owns_data() = owns_data;
  _capacity()  = capacity;
  _capacity_inc() = capacity_inc;
}

template <class TRAITS>
void  vector<TRAITS>::set_capacity(int new_capacity)
{
  UTIL_ASSERT(size() <= new_capacity);
  _data() = Traits::realloc_data_array(data(), size(),
                                      _capacity() = new_capacity);
  UTIL_ASSERT(data() != 0);
}

template <class TRAITS>
void  vector<TRAITS>::ensure_insert()
{
  if (size() == capacity()) {
    UTIL_ASSERT(0 <= capacity_inc());
    UTIL_ASSERT(0 < capacity() || 0 < capacity_inc());
    set_capacity((0 < capacity_inc()) ? (capacity() + capacity_inc())
                                      : (2 * capacity()));
  }
}

template <class TRAITS>
void  vector<TRAITS>::copy (const vector& v)
{
  if (_header->nref == 0) { // we hold a _header no one else points to
    if (this == &v) {       // and we are asked to copy ourself ?!
      return;               // well nothing left to do then ...
    }
    flush();                // since nobody aliases it
  }
  else {
    drop();                 // drop it to the fellows
    cons_header(default_capacity(), default_capacity_inc(), false);
  }

  insert(begin(), v.begin(), v.end());
}

template <class TRAITS>
void  vector<TRAITS>::insert(iterator pos,
                             const_iterator begin, const_iterator end)
{
  int nelem = end - begin;
  if (capacity() < size() + nelem) {
    set_capacity(size() + nelem);
  }
  Traits::insert_entry(nelem, data() + pos.index, size() - pos.index);
  for (Data* put = data() + pos.index; begin != end; ++put, ++begin) {
    *put = *begin;
  }
  _size() += nelem;
}

#ifndef UTIL_VECTORT_H
# include "util/vectort.h"
#endif

#endif //UTIL_VECTOR_H
