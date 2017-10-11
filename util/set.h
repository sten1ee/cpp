#ifndef UTIL_SET_H
#define UTIL_SET_H

#include <string.h>

#if !defined(SET_AS_SORTED_VECTOR) && !defined(SET_AS_AVL_TREE)
# define SET_AS_SORTED_VECTOR
#endif

#ifdef SET_AS_AVL_TREE
# include "util/avl_tree.h"
#else
# include "util/sortvec.h"
#endif


template <class TRAITS>
class set : public
#ifdef SET_AS_AVL_TREE
avl_tree
#else
sorted_vector
#endif
<TRAITS>
{
  typedef
#ifdef SET_AS_AVL_TREE
avl_tree
#else
sorted_vector
#endif
<TRAITS>  super;

  public:
    set() {;}
    set(const set& s) : super(s) {;}
    set(bool owns_data) : super(owns_data) {;}
};

template <class TRAITS>
struct set_traits : public
#ifdef SET_AS_AVL_TREE
avl_traits
#else
sortvec_traits
#endif
<TRAITS>
{};

template <class TRAITS>
struct set_traits_ptr : public
#ifdef SET_AS_AVL_TREE
avl_traits_ptr
#else
sortvec_traits_ptr
#endif
<TRAITS>
{};


#ifdef  SET
#  ifdef  SET_OLD_VALUE
#    error this really sux ...
#  endif
#  define SET_OLD_VALUE SET
#  undef  SET
#endif

#define SET set
#  include "util/setoper.h"
#undef  SET

#ifdef  SET_OLD_VALUE
#  define SET SET_OLD_VALUE
#  undef  SET_OLD_VALUE
#endif

/**********************************************************************
#define DEFINE_NAMED_SET_BY_CSTR_KEY(SET_NAME, CLASS, KEY_GETTER) \
struct SET_NAME##_traits_ : set_traits< CLASS >\
{\
  typedef const char* DataKey;\
  static DataKey keyof(DataIn data) { return data.KEY_GETTER; }\
  static int     compare(DataKey k1, DataKey k2) { return strcmp(k1, k2); }\
};\
typedef set< SET_NAME##_traits_ > SET_NAME

#define DEFINE_NAMED_PTR_SET_BY_CSTR_KEY(SET_NAME, CLASS, KEY_GETTER) \
struct SET_NAME##_traits_ : set_traits_ptr< CLASS >\
{\
  typedef const char* DataKey;\
  static DataKey keyof(DataIn data) { return data->KEY_GETTER; }\
  static int     compare(DataKey k1, DataKey k2) { return strcmp(k1, k2); }\
};\
typedef set< SET_NAME##_traits_ > SET_NAME

#define DEFINE_SET_BY_CSTR_KEY(CLASS, KEY_GETTER) \
        DEFINE_NAMED_SET_BY_CSTR_KEY(set, CLASS, KEY_GETTER)

#define DEFINE_PTR_SET_BY_CSTR_KEY(CLASS, KEY_GETTER) \
        DEFINE_NAMED_PTR_SET_BY_CSTR_KEY(set, CLASS, KEY_GETTER)
**********************************************************************/

#endif //UTIL_SET_H
