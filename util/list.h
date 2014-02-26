#ifndef UTIL_LIST_H
#define UTIL_LIST_H

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif
#if  defined(OPTION_CUSTOM_ALLOCATOR)
#    include "util/callocat.h"
#endif
#include "util/listt.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class TRAITS>
struct list_node
{
    typename TRAITS::Data  data;
    list_node*    next;

    list_node(typename TRAITS::DataIn _data, list_node* _next)
      : data(_data), next(_next)
    {;}
#ifdef OPTION_CUSTOM_ALLOCATOR
    void* operator  new(size_t sz);
    void  operator  delete(void* p);
#endif
};

#ifdef OPTION_CUSTOM_ALLOCATOR
template <class TRAITS>
inline
void* list_node<TRAITS>::operator new(size_t sz)
{
  return  custom_allocator<sizeof(list_node)>::allocate();
}

template <class TRAITS>
inline
void  list_node<TRAITS>::operator delete(void* ptr)
{
  custom_allocator<sizeof(list_node)>::deallocate(ptr);
}
#endif//OPTION_CUSTOM_ALLOCATOR

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class TRAITS> class list
{
  public:
    typedef TRAITS   Traits;
    typedef list_node<TRAITS>            node_type;
    typedef typename Traits::Data        Data;
    typedef typename Traits::DataIn      DataIn;

    class const_iterator
    {
      protected:

        friend  class list<TRAITS>;

        node_type**   ppnode;

        const_iterator(node_type** _ppnode) : ppnode(_ppnode) {;}

      public:
        const_iterator()                           { ppnode = 0; }
        const_iterator(const const_iterator& iter) { ppnode = iter.ppnode; }

        const_iterator& operator = (const const_iterator& iter)
                                       { ppnode = iter.ppnode; return *this; }
        bool    operator == (const const_iterator& iter) const
                                       { return ppnode == iter.ppnode; }

        bool    operator != (const const_iterator& iter) const
                                       { return ppnode != iter.ppnode; }

        bool    initialized() const { return ppnode != 0; }

        const_iterator& operator ++ ()
          { ppnode = &(*ppnode)->next; return *this; }
        const_iterator  operator ++ (int)
          { const_iterator tmp = *this; ppnode = &(*ppnode)->next; return tmp; }

        const Data& operator * () const      { return (*ppnode)->data;  }
    };

    class iterator : public const_iterator
    {
      protected:

        friend  class list<TRAITS>;
        friend  class const_iterator;

        iterator(node_type** _ppnode) : const_iterator(_ppnode) {;}

      public:
        iterator()                     : const_iterator() {;}
        iterator(const iterator& iter) : const_iterator(iter) {;}

        iterator& operator = (const iterator& iter)
                                       { ppnode = iter.ppnode; return *this; }

        iterator& operator ++ ()
          { ppnode = &(*ppnode)->next; return *this; }
        iterator  operator ++ (int)
          { iterator tmp = *this; ppnode = &(*ppnode)->next; return tmp; }

        Data& operator * () const      { return (*ppnode)->data;  }
    };

  private:

    struct  header_type
    {
      node_type*   head;
      node_type**  rear_ptr;
      int          size;
      int          nref;
      bool         owns_data;
    };

    header_type* _header;

    node_type*&  _head     ()       { return _header->head;     }
    node_type**& _rear_ptr ()       { return _header->rear_ptr; }
    int&         _size     ()       { return _header->size;     }
    bool&        _owns_data()       { return _header->owns_data;}

  protected:

    node_type* const &   head     () const { return _header->head;     }
    node_type** const &  rear_ptr () const { return _header->rear_ptr; }

    void  drop()
      {
        if (_header->nref == 0) {
          flush();
          delete _header;
        }
        else {
          _header->nref--;
        }
        UTIL_DEBUGIN(_header = 0;)
      }

    void  join(const list& list)
      {
        (_header = list._header)->nref++;
      }

  public:
          iterator  begin ()       { return       iterator(&_head());   }
    const_iterator  begin () const { return const_iterator(&head());    }
          iterator  end   ()       { return       iterator(rear_ptr()); }
    const_iterator  end   () const { return const_iterator(rear_ptr()); }

          void    insert_front(DataIn data);
          void    insert_back(DataIn data);
          void    add(DataIn data)  { insert_back(data);  }
          void    insert(const iterator& position, DataIn data);
          void    insert(const_iterator beg, const_iterator end);
          void    pop_front();
          void    erase(const iterator& position);
          void    flush(bool delData);
          void    flush()           { flush(owns_data()); }
          bool    empty()     const { return head() == 0; }
          bool    owns_data() const { return _header->owns_data; }
          void    owns_data(bool od){ _header->owns_data = od;   }
    const int&    size  ()    const { return _header->size;      }
          Data&   front();
    const Data&   front()     const;
          Data&   back();
    const Data&   back()      const;

    void  copy (const list& list);
    void  alias(const list& list);
    void  construct_empty();

         ~list();
          list()  { construct_empty(); }
          list(const list& list)
            {
#ifdef  OPTION_HARD_COPY
              construct_empty();
              insert(list.begin(), list.end());
#else
              (_header = list._header)->nref++;
#endif
            }

    list& operator = (const list& list)
            {
#ifdef  OPTION_HARD_COPY
              copy(list);
#else
              alias(list);
#endif
              return *this;
            }
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class TRAITS>
list<TRAITS>::~list()
{
  if (_header->nref == 0) {
    flush();
    delete _header;
  }
  else {
    _header->nref--;
  }
}

template <class TRAITS>
void  list<TRAITS>::construct_empty()
{
  (_header = new header_type)->nref = 0;
  _size() = 0;
  _head() = 0;
  _rear_ptr()  = &_head();
  _owns_data() = Traits::default_owns_data();
}

template <class TRAITS>
void  list<TRAITS>::alias(const list& list)
{
  if (_header != list._header) {
    if (_header->nref == 0) {
      flush();
    }
    else {
      _header->nref--;
    }
    (_header = list._header)->nref++;
  }
}

template <class TRAITS>
void  list<TRAITS>::copy(const list& tree)
{
  if (_header->nref == 0) { // we hold a _header no one else points to
    if (this == &tree) {    // and we are asked to copy ourself ?!
      return;               // well nothing left to do then ...
    }
    flush();                // since nobody aliases it
  }
  else {
    _header->nref--;        // drop it to the fellows
    construct_empty();
  }

  insert(tree.begin(), tree.end());
}

template <class TRAITS>
void  list<TRAITS>::flush(bool owns_data)
{
  while (head() != 0) {
    node_type* pndel = head();
    _head() = pndel->next;
    Traits::destroy_data(pndel->data, owns_data);
    delete pndel;
  }
  _rear_ptr() = &_head();
  _size() = 0;
}

template <class TRAITS>
void  list<TRAITS>::insert_back(DataIn data) //add data at tail
{
  node_type* pnnew = new node_type(data, 0);
 *rear_ptr() =  pnnew;
  _rear_ptr() = &pnnew->next;
}

template <class TRAITS>
void  list<TRAITS>::insert_front(DataIn data) //possible but not usual
{
  if (head() == 0)
    insert_back(data);
  else
    _head() = new node_type(data, head());
}

template <class TRAITS>
void  list<TRAITS>::pop_front()
{
  UTIL_ASSERT(head() != 0);
  node_type* pndel= head();
  if (0 == (_head() = head()->next)) {
    _rear_ptr() = &_head();
  }
  delete pndel;
}

template <class TRAITS>
void  list<TRAITS>::erase(const iterator& position)
{
  node_type* pndel = *position.ppnode;
  if (0 == (*position.ppnode = pndel->next)) {
    _rear_ptr() = position.ppnode;
  }
  delete pndel;
}

template <class TRAITS>
void  list<TRAITS>::insert(const iterator& position, DataIn data)
{
  node_type* pnnew = *position.ppnode = new node_type(data, *position.ppnode);
  if (0 == pnnew->next) {
    _rear_ptr() = &pnnew->next;
  }
}

template <class TRAITS>
inline
typename list<TRAITS>::Data& list<TRAITS>::front()
{
  UTIL_ASSERT(head() != 0);
  return head()->data;
}

template <class TRAITS>
inline
const typename list<TRAITS>::Data& list<TRAITS>::front() const
{
  UTIL_ASSERT(head() != 0);
  return head()->data;
}

template <class TRAITS>
inline
typename list<TRAITS>::Data& list<TRAITS>::back()
{
  UTIL_ASSERT(head() != 0);
  return ((node_type*)((char*)rear_ptr() - offsetof(node_type, next)))->data;
}

template <class TRAITS>
inline
const typename list<TRAITS>::Data& list<TRAITS>::back() const
{
  UTIL_ASSERT(head() != 0);
  return ((node_type*)((char*)rear_ptr() - offsetof(node_type, next)))->data;
}

#endif//UTIL_LIST_H
