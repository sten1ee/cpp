#ifndef UTIL_AVL_TREE_H
#define UTIL_AVL_TREE_H

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif
#ifdef    OPTION_CUSTOM_ALLOCATOR
#include "util/callocat.h"
#endif

#ifndef   DEBUGGER_DECL
#define   DEBUGGER_DECL
#endif

struct avl_node_base
{
    DEBUGGER_DECL

    friend class avl_tree_base;

  private:
    int             len;

  public:
    avl_node_base*  parent;
    avl_node_base*  left;
    avl_node_base*  right;

    avl_node_base() {;}
};


class avl_tree_base
{
    DEBUGGER_DECL

  protected:
    typedef  avl_node_base* link_type;

  private:
    struct header_type : avl_node_base
    {
      link_type   rightmost;
      int         size;
      bool        owns_data;
    };

    header_type*  _header;

          void       operator = (const avl_tree_base& tree) {;}

          int&      _size      ()       { return _header->size;      }
          int&      _nref      () const { return _header->len;       }

  protected:
          link_type&_root      ()       { return _header->left;      }
          link_type&_leftmost  ()       { return _header->right;     }
          link_type&_rightmost ()       { return _header->rightmost; }

    const link_type  header    () const { return _header;            }
    const link_type  root      () const { return _header->left;      }
    const link_type  leftmost  () const { return _header->right;     }
    const link_type  rightmost () const { return _header->rightmost; }

          int        nref      () const { return _header->len;       }

  public:
    const int&       size      () const { return _header->size;      }
          bool       empty     () const { return size() == 0;        }
    const bool&      owns_data () const { return _header->owns_data; }
          void       owns_data (bool od){ _header->owns_data = od;   }
          int        max_depth () const { return root() ? root()->len : 0; }

    // the following 3 methods are used by tree-iterator classes:
    static  link_type  next(link_type node);
    static  link_type  prev(link_type node);
    static  bool       is_header(const avl_node_base* node)
                                              { return node->parent == node; }
  protected:
    static  link_type  parent(link_type node) { return node->parent; }
    static  link_type  left  (link_type node) { return node->left  ; }
    static  link_type  right (link_type node) { return node->right ; }

    void  drop() const
      {
        if (nref() == 0) {
          delete _header; // we are the only owner of this tree !
        }
        else {
          ++_nref(); // actually decrement
        }
      }

    void  join(const avl_tree_base& tree)
      {
        _header = tree._header;
        --_nref(); // actually increment
      }

    void  flushed()
      {
        _root() = 0;
        _leftmost() = _rightmost() = _header;
        _size() = 0;
      }

    void  construct_empty()
      {
        _header = new header_type;
        _header->parent = _header;
        _nref() = 0;
        owns_data(true);
        flushed();
      }

    void  construct_alias(const avl_tree_base& tree)
      {
        join(tree);
      }

   ~avl_tree_base()
      {
        drop();
      }

    void  swap(avl_tree_base& tree)
    { // doesn't really matter if this->_header == tree._header
      header_type* tmp = _header;
      _header = tree._header;
      tree._header = tmp;
    }

    void  insert(link_type parent, link_type* ppchild, link_type new_node);

    void  erase(link_type del_node);

    static link_type  rotate(link_type b, bool rot_right);
};

/*******************************************************************************
struct  ADT
{
  int key;
  // etc ...
};

struct  ADT_Traits
{
  // Data - the actual datatype stored in the tree:
  typedef       ADT   Data;

  // DataIn - the argument type of the insert functions:
  typedef const ADT&  DataIn;
  // -- or better (for primitive types) :
  typedef       ADT   DataIn;

  // DataKey - the argument type of the find functions:
  typedef       int   DataKey; // (may be: const T& as well)

  // required methods:
  static  int   compare(DataKey key, DataIn adt) { return key - adt.key; }
  static  int   compare(DataIn adt1, DataIn adt2){ return adt1.key - adt2.key; }
  static  bool  allow_duplicates()               { return true/false; }
  static  void  destroy(Data& adt) { ...; }
};
*******************************************************************************/

template <class TRAITS>
struct avl_node : public avl_node_base
{
    DEBUGGER_DECL

  private:
    typedef typename TRAITS::Data           Data;
    typedef typename TRAITS::DataIn         DataIn;

  public:
    Data   data;

    avl_node*&  parent() const { return (avl_node*&)avl_node_base::parent; }
    avl_node*&  left  () const { return (avl_node*&)avl_node_base::left;   }
    avl_node*&  right () const { return (avl_node*&)avl_node_base::right;  }

    avl_node*&  parent()  { return (avl_node*&)avl_node_base::parent; }
    avl_node*&  left  ()  { return (avl_node*&)avl_node_base::left;   }
    avl_node*&  right ()  { return (avl_node*&)avl_node_base::right;  }

    avl_node(DataIn _data)
      : data(_data)
      {;}

#ifdef OPTION_CUSTOM_ALLOCATOR
    void* operator  new(size_t sz);
    void  operator  delete(void* p);
#endif
};


#ifdef OPTION_CUSTOM_ALLOCATOR
template <class TRAITS>
inline
void* avl_node<TRAITS>::operator new(size_t sz)
{
  return  custom_allocator<sizeof(avl_node)>::allocate();
}

template <class TRAITS>
inline
void  avl_node<TRAITS>::operator delete(void* ptr)
{
  custom_allocator<sizeof(avl_node)>::deallocate(ptr);
}
#endif//OPTION_CUSTOM_ALLOCATOR

template <class TRAITS>
class avl_tree : public avl_tree_base
{
    DEBUGGER_DECL

  private:
    typedef  avl_node<TRAITS>         node_type;

  public:
    typedef           TRAITS          Traits;
    typedef  typename Traits::Data    Data;
    typedef  typename Traits::DataIn  DataIn;
    typedef  typename Traits::DataKey DataKey;

    class finger;
    class iterator;
    class const_iterator;

    class finger
    {
        DEBUGGER_DECL

        friend class avl_tree<TRAITS>;
        friend class iterator;
        friend class const_iterator;

        avl_node_base*  parent;
        node_type**     ppchild;

        finger(avl_node_base* _parent, node_type** _ppchild)
            : parent(_parent), ppchild(_ppchild) {;}

      public:
        finger() : parent(0), ppchild(0) {}

        // does this finger points to data ?
        Data& operator  *    ()              const { return (*ppchild)->data; }
              operator  bool ()              const { return 0 != *ppchild;    }
        bool  operator  == (const finger& i) const { return ppchild == i.ppchild; }
    };

    class const_iterator
    {
        DEBUGGER_DECL

        friend class avl_tree<TRAITS>;

      protected:
        // note that 'node' is declared of type 'node_type*' rather than
        // 'const node_type*' to avoid a bunch of const_cast(s) in the
        // derived 'iterator' class;
        // nevertheless its usage in this class is strictly as
        // a pointer to const data !
        node_type* node;

        const_iterator(const node_type* _node)  : node(const_cast<node_type*>(_node)) {;}

      public:
        const_iterator()                        : node(0)           {;}
        const_iterator(const const_iterator& i) : node(i.node)      {;}
        const_iterator(const finger&  fi)       : node(*fi.ppchild) {;}

        const Data&     operator * () const { UTIL_ASSERT(!is_header(node)); return node->data; }
        const_iterator  operator ++()       { node = (node_type*)next((node_type*)node); return *this; }
        const_iterator  operator --()       { node = (node_type*)prev((node_type*)node); return *this; }
        const_iterator  operator ++(int)    { const_iterator res = *this; node = (node_type*)next((node_type*)node); return res; }
        const_iterator  operator --(int)    { const_iterator res = *this; node = (node_type*)prev((node_type*)node); return res; }
        bool            operator ==  (const const_iterator& i) const { return node == i.node; }
        bool            operator !=  (const const_iterator& i) const { return node != i.node; }

        bool            has_parent()  const { return !is_header(node->parent());    }
        bool            has_left  ()  const { return 0 != node->left();             }
        bool            has_right ()  const { return 0 != node->right();            }

        const_iterator  parent    ()  const { UTIL_ASSERT(has_parent()); return const_iterator(node->parent());}
        const_iterator  left      ()  const { UTIL_ASSERT(has_left())  ; return const_iterator(node->left());  }
        const_iterator  right     ()  const { UTIL_ASSERT(has_right()) ; return const_iterator(node->right()); }
    };

    class iterator : public const_iterator
    {
        DEBUGGER_DECL

        friend class avl_tree<TRAITS>;
        friend class const_iterator;

      protected:
        iterator(node_type* _node)  : const_iterator(_node)       {;}

      public:
        iterator()                  : const_iterator()            {;}
        iterator(const iterator& i) : const_iterator(i)           {;}
        iterator(const finger& fi)  : const_iterator(*fi.ppchild) {;}

        Data&     operator * ()     { UTIL_ASSERT(!is_header(node)); return node->data; }
        iterator& operator ++()     { node = (node_type*)next(node); return *this; }
        iterator& operator --()     { node = (node_type*)prev(node); return *this; }
        iterator  operator ++(int)  { iterator res = *this; node = (node_type*)next(node); return res; }
        iterator  operator --(int)  { iterator res = *this; node = (node_type*)prev(node); return res; }

        iterator  parent    ()  const { UTIL_ASSERT(has_parent()); return iterator(node->parent()); }
        iterator  left      ()  const { UTIL_ASSERT(has_left())  ; return iterator(node->left())  ; }
        iterator  right     ()  const { UTIL_ASSERT(has_right()) ; return iterator(node->right()) ; }
    };

    iterator        begin()       { return iterator((node_type*)(leftmost())); }
    iterator        end()         { return iterator((node_type*)(header())); }

    const_iterator  begin() const { return const_iterator((const node_type*)(leftmost())); }
    const_iterator  end()   const { return const_iterator((const node_type*)(header())); }

    const Data&     front() const { UTIL_ASSERT(!empty()); return ((node_type*)leftmost())->data; }
    const Data&     back()  const { UTIL_ASSERT(!empty()); return ((node_type*)rightmost())->data; }

    void  erase(iterator i)
        {
          avl_tree_base::erase(i.node);
          delete i.node;
        }

    void  erase(iterator i, iterator j);

    void  erase_front()
        {
          UTIL_ASSERT(!empty());
          avl_tree_base::erase(leftmost());
        }

    void  erase_back()
        {
          UTIL_ASSERT(!empty());
          avl_tree_base::erase(rightmost());
        }

    void  flush(bool destroy_data);
    void  flush() { flush(owns_data()); }

    enum  copy_tag  { COPY };
    enum  alias_tag { ALIAS };

    avl_tree();
    avl_tree(bool owns_data);     
    avl_tree(const avl_tree& tree);
    avl_tree(const avl_tree& tree, copy_tag);
    avl_tree(const avl_tree& tree, alias_tag);
   ~avl_tree() {if (nref() == 0) flush();}

    void  copy (const avl_tree& tree);
    void  alias(const avl_tree& tree);
    void  swap (      avl_tree& tree)   { avl_tree_base::swap(tree); }

#ifdef  OPTION_HARD_COPY
    void  assign(const avl_tree& tree)  { copy(tree);  }
#else // make only alias upon assignment:
    void  assign(const avl_tree& tree)  { alias(tree); }
#endif
    avl_tree& operator = (const avl_tree& tree) { assign(tree);  return *this; }

    finger  find(DataKey key);
    void    insert(finger& f, DataIn data);
    
    // assumes data is not equal to any element in the sorted vector
    // or duplicates are allowed.
    void    insert(DataIn data);
    // if  data is not equal to any element already stored
    // inserts it & returns true; otherwise returns false.
    bool    insert_unique(DataIn data);

    void    insert_back(DataIn data);
    void    insert(const_iterator i, const_iterator j);

    // this is < tree, == tree, > tree  if compare returns < 0, == 0, > 0;
    int     compare(const avl_tree& tree) const;
    // do the two trees contain the same data elements ?
    bool    operator == (const avl_tree& tree) const { return 0 == compare(tree); }
};


template <class TRAITS>
avl_tree<TRAITS>::avl_tree()
{
  construct_empty();
}


template <class TRAITS>
avl_tree<TRAITS>::avl_tree(bool od)
{
  construct_empty();
  owns_data(od);
}


template <class TRAITS>
avl_tree<TRAITS>::avl_tree(const avl_tree& tree)
{
#ifdef  OPTION_HARD_COPY
  construct_empty();
  owns_data(false);
  insert(tree.begin(), tree.end());
#else
  construct_alias(tree);
#endif
}


template <class TRAITS>
avl_tree<TRAITS>::avl_tree(const avl_tree& tree, copy_tag)
{
  construct_empty();
  owns_data(false);
  insert(tree.begin(), tree.end());
}


template <class TRAITS>
avl_tree<TRAITS>::avl_tree(const avl_tree& tree, alias_tag)
{
  construct_alias(tree);
}


template <class TRAITS>
void  avl_tree<TRAITS>::alias(const avl_tree& tree)
{
  if (header() != tree.header()) {
    if (nref() == 0) {
      // destroy the constituent nodes and (optionally) data:
      flush();
    }
    drop();
    join(tree);
  }
}


template <class TRAITS>
void  avl_tree<TRAITS>::copy(const avl_tree& tree)
{
  if (nref() == 0) {     // we hold a _header no one else points to
    if (this == &tree) { // and we are asked to copy ourself ?!
      return;            // well nothing left to do then ...
    }
    flush();             // since nobody aliases it
  }
  else {
    drop();              // drop it to the fellows
    construct_empty();
  }

  insert(tree.begin(), tree.end());
}


template <class TRAITS>
void  avl_tree<TRAITS>::insert(DataIn data)
{
  finger  fi = find(Traits::keyof(data));
  UTIL_ASSERT(!fi || Traits::allow_duplicates());
  insert(fi, data);
}


template <class TRAITS>
bool  avl_tree<TRAITS>::insert_unique(DataIn data)
{
  finger  fi = find(Traits::keyof(data));
  if (fi)
    return false;
  insert(fi, data);
  return true;
}


template <class TRAITS>
void  avl_tree<TRAITS>::erase(iterator i, iterator j)
{
  while (i != j) {
    erase(i++);
    // note:
    // it is important that we do it exactly this way:
    // erase(i++);
    // and not that way:
    // erase(i); ++i;
    // because after 'erase(i)', 'i' will point to deleted node
    // hence ++i is undefined !
  }
}


template <class TRAITS>
void  avl_tree<TRAITS>::flush(bool destroy_data)
{
  if (empty()) {
    return;
  }
  // note again that 42 plays a major role in our universe:
  // fib(42) is the smallest fib >= 2^28 !!!
  // and in a 32-bit address space no tree can contain more than
  // 2^28 nodes as each node takes at least 16 == 2^4 bytes !
  link_type  node_stack[42];
  link_type* top_node = node_stack;

  *top_node = root();

  while (true) { // unroll_left
    while ((*top_node)->left != 0) {
      *(top_node + 1)= (*top_node)->left;
      ++top_node;
    }
    while (true) { // unroll_right:
      node_type* del_node = static_cast<node_type*>(*top_node);
      link_type  rht_node = (*top_node)->right;

      if (destroy_data) {
        Traits::destroy(del_node->data);
      }
      delete del_node;

      if (rht_node != 0) {
        *top_node = rht_node;
        break; // (ie. goto unroll_left)
      }
      else {
        if (top_node == node_stack) {
          avl_tree_base::flushed();
          return;
        }
        --top_node;
        // (ie. goto unroll_right)
      }
    }
  }
}


template <class TRAITS>
avl_tree<TRAITS>::finger
avl_tree<TRAITS>::find(DataKey key)
{
  link_type  parent  =  header();
  node_type**ppchild = (node_type**)&_root();
  while (*ppchild != 0) {
    int cmp_res = Traits::compare(key, Traits::keyof((*ppchild)->data));
    if (cmp_res == 0) {
      break;
    }
    parent = *ppchild;
    if (cmp_res < 0) {
      ppchild = &(*ppchild)->left();
    }
    else {
      ppchild = &(*ppchild)->right();
    }
  }
  return  finger(parent, ppchild);
}


template <class TRAITS>
void  avl_tree<TRAITS>::insert(finger& f, DataIn data)
{
  UTIL_ASSERT(!f);

  link_type  node = new node_type(data);
  avl_tree_base::insert(f.parent, (link_type*)f.ppchild, node); // notify base
  f.ppchild = (node_type**)(node == node->parent->left ? &node->parent->left
                                                       : &node->parent->right);

  UTIL_DEBUGIN(link_type  aux;)
  UTIL_ASSERT((aux = prev(node)) == end().node
      || Traits::compare(Traits::keyof(((node_type*)aux)->data),
                         Traits::keyof(data)) <= 0);
  UTIL_ASSERT((aux = next(node)) == end().node
      || Traits::compare(Traits::keyof(data),
                         Traits::keyof(((node_type*)aux)->data)) <= 0);
}


template <class TRAITS>
void  avl_tree<TRAITS>::insert_back(DataIn data)
{
  if (empty()) {
    avl_tree_base::insert(header(), &_root(), new node_type(data));
  }
  else {
    UTIL_ASSERT(Traits::compare(Traits::keyof(((node_type*)rightmost())->data),
                                Traits::keyof(data)) <= 0);
    avl_tree_base::insert(rightmost(), &rightmost()->right, new node_type(data));
  }
}


template <class TRAITS>
void  avl_tree<TRAITS>::insert(const_iterator i, const_iterator j)
{
  while (i != j) {
    insert(*i);
    ++i;
  }
}


template <class TRAITS>
int   avl_tree<TRAITS>::compare(const avl_tree& tree) const
{
  int size_diff = size() - tree.size();
  if (size_diff) {
    return size_diff;
  }
  const_iterator i1 = begin(), end1 = end();
  const_iterator i2 = tree.begin();
  while (i1 != end1) {
    int diff = Traits::compare(Traits::keyof(*i1), Traits::keyof(*i2));
    if (diff) {
      return diff;
    }
  }
  return 0;
}


template <class DATA>
struct  avl_traits
{
  typedef       DATA   Data;
  typedef const DATA&  DataIn;
  typedef const DATA&  DataKey;

  // default behaviour:
  static  bool    allow_duplicates() { return false; }
  static  void    destroy(Data& adt) {;}
  static  DataKey keyof(DataIn data) { return data; }
};


template <class DATA>
struct  avl_traits_ptr
{
  typedef       DATA*  Data;
  typedef       DATA*  DataIn;
  typedef const DATA*  DataKey;

  // default behaviour:
  static  bool    allow_duplicates() { return false; }
  static  void    destroy(DATA* ptr) { delete ptr;}
  static  DataKey keyof(DataIn data) { return data; }
};


template <class TRAITS>
struct avl_traits_avl_tree : public avl_traits< avl_tree< TRAITS > >
{
  int compare(DataIn tree1, DataIn tree2)
    {
      return tree1.compare(tree2);
    }
};


#ifdef  AVL_TREE_SETOPER
#  ifdef  SET
#    ifdef  SET_OLD_VALUE
#      error this really sux ...
#    endif
#    define SET_OLD_VALUE SET
#    undef  SET
#  endif

#  define SET avl_tree
#    include "util/setoper.h"
#  undef  SET

#  ifdef  SET_OLD_VALUE
#    define SET SET_OLD_VALUE
#    undef  SET_OLD_VALUE
#  endif
#endif

#endif //UTIL_AVL_TREE_H
