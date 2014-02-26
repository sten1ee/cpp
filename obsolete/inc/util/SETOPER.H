#ifndef UTIL_SETOPER_H
#define UTIL_SETOPER_H

#ifndef   UTIL_SORTVEC_H
#include "util/sortvec.h"
#endif

enum SO { //set operation
  SO_AND,
  SO_OR ,
  SO_XOR,
  SO_SUB
};

template <class TRAITS>
SortedVector<TRAITS>  SetOperation(int so,
              const SortedVector<TRAITS>& set1,
              const SortedVector<TRAITS>& set2);
template <class TRAITS>
bool isSubset(const SortedVector<TRAITS>& set1,
              const SortedVector<TRAITS>& set2);
template <class TRAITS>
bool isEqual( const SortedVector<TRAITS>& set1,
              const SortedVector<TRAITS>& set2);

template <class TRAITS>
inline
SortedVector<TRAITS>  operator - (const SortedVector<TRAITS>& set1,
                                  const SortedVector<TRAITS>& set2)
{
  return SetOperation(SO_SUB, set1, set2);
}

template <class TRAITS>
inline
SortedVector<TRAITS>  operator ^ (const SortedVector<TRAITS>& set1,
                                  const SortedVector<TRAITS>& set2)
{
  return SetOperation(SO_XOR, set1, set2);
}

template <class TRAITS>
inline
SortedVector<TRAITS>  operator & (const SortedVector<TRAITS>& set1,
                                  const SortedVector<TRAITS>& set2)
{
  return SetOperation(SO_AND, set1, set2);
}

template <class TRAITS>
inline
SortedVector<TRAITS>  operator | (const SortedVector<TRAITS>& set1,
                                  const SortedVector<TRAITS>& set2)
{
  return SetOperation(SO_OR, set1, set2);
}

template <class TRAITS>
SortedVector<TRAITS>  SetOperation(int so,
                const SortedVector<TRAITS>& set1,
                const SortedVector<TRAITS>& set2)
{
  SortedVector<TRAITS> resSet(!OWNS_DATA);
  size_t i1 = 0;
  size_t i2 = 0;
  while (i1 < set1.size() && i2 < set2.size()) {
    int diff = TRAITS::Compare(set1[i1], set2[i2]);
    switch (so) {
      case SO_AND:
        if (diff == 0) goto ADD_SET1_ELEMENT;
        break;

      case SO_XOR:
        if (diff != 0) goto ADD_LESS_ELEMENT;
        break;

      case SO_SUB:
        if (diff < 0) goto ADD_SET1_ELEMENT;
        break;

      case SO_OR: goto ADD_LESS_ELEMENT;

      default:
        UTIL_ASSERT(0); //unknown set operation!
      ADD_LESS_ELEMENT:
        if (diff <= 0) {
          ADD_SET1_ELEMENT:
            resSet.push(set1[i1]);
        }
        else {
          resSet.push(set2[i2]);
        }
    }
    if (diff <= 0) i1++;
    if (diff >= 0) i2++;
  }
  switch (so) {
    case SO_XOR:
    case SO_OR :
      while (i2 < set2.size()) {
        resSet.push(set2[i2++]);
      }

    case SO_SUB:
      while (i1 < set1.size()) {
        resSet.push(set1[i1++]);
      }
  }

  return resSet;
}


template <class TRAITS>
bool isSubset(const SortedVector<TRAITS>& set1,
              const SortedVector<TRAITS>& set2)
{
  size_t i1 = set1.size();
  size_t i2 = set2.size();

  while (0 < i1--) {
    while (0 < i2--) {
      if (i1 > i2) {
        return false;
      }
      int res = TRAITS::Compare(set1[i1], set2[i2]);
      if (res == 0) { // set1[i1] == set2[i2]
        goto NEXT_I1;
      }
      if (res > 0) {  // no match for set1[i1] in set2
        return false;
      }
    }
    return false;
  NEXT_I1:
    ; //some compilers issue an error if label is not followed by statement
  }
  return true;
}


template <class TRAITS>
bool isEqual(const SortedVector<TRAITS>& set1,
             const SortedVector<TRAITS>& set2)
{
  if (set1.size() != set2.size()) {
    return false;
  }
  for (size_t i = set1.size(); 0 < i--; ) {
    if (0 != TRAITS::Compare(set1[i], set2[i])) {
      return false;
    }
  }
  return true;
}

#endif //UTIL_SETOPER_H
