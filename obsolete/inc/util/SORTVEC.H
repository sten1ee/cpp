#ifndef UTIL_SORTVEC_H
#define UTIL_SORTVEC_H

#ifndef   UTIL_VECTOR_H
#include "util/vector.h"
#endif

const size_t  NOT_FOUND = size_t(-1);


template <class TRAITS> class SortedVector : protected Vector<TRAITS>
{
  public:
    typedef Vector<TRAITS>  Parent;
    typedef typename TRAITS::DataId  DataId;

    Parent::trimToSize;
    Parent::getCapacity;
    Parent::size;
    Parent::isEmpty;
    Parent::ownsData;
    Parent::ensureInsertion;
    Parent::setCapacity;
    Parent::removeAt;
    Parent::pop;
    Parent::flush;
    Parent::operator [];

    SortedVector(size_t share=OWNS_DATA)
      : Parent(share)
      {}

    SortedVector(size_t initialCapacity, int capacityIncrement)
      : Parent(initialCapacity, capacityIncrement)
      {}

    SortedVector(SortedVector& sv)
      : Parent(sv)
      {}

    SortedVector&  assignGut(SortedVector& sv)
      {
        Parent::assignGut(sv);
        return *this;
      }

#ifdef OPTION_NO_NONCONST_ASSIGN
    SortedVector&  operator = (SortedVector  sv)
#else
    SortedVector&  operator = (SortedVector& sv)
#endif
      {
        return  assignGut(sv);
      }

    const Vector<TRAITS>& toVector() const { return *this; }

    bool    isSorted();
    size_t  indexOf(DataId stub);
    bool    insertIfNotExists(Data obj);
    bool    removeIfExists(DataId stub);

    // assumes obj is not equal to any element in the sorted vector:
    void    insert(Data obj)
      {
        UTIL_DEBUGIN(bool res = )
          insertIfNotExists(obj);
        UTIL_ASSERT(res);
      }
    // assumes obj is greater than any element in the sorted vector:
    void    insertBack(Data obj)
      {
        UTIL_ASSERT(isEmpty() || Traits::Compare(data[dataCount-1], obj) < 0);
        push(obj);
      }
    // assumes obj is equal to some element in the sorted vector:
    void    remove(DataId stub)
      {
        UTIL_DEBUGIN(bool res = )
          removeIfExists(stub);
        UTIL_ASSERT(res);
      }

    friend SortedVector<TRAITS> SetOperation(int set_operation_code,
                                       const SortedVector<TRAITS>& arg_set1,
                                       const SortedVector<TRAITS>& arg_set2);
};

template <class TRAITS>
bool  SortedVector<TRAITS>::isSorted()
{
  for (size_t i = dataCount; 1 < i--; ) {
    if (0 <= TRAITS::Compare(data[i-1], data[i])) {
      return false;
    }
  }
  return true;
}

template <class TRAITS>
size_t  SortedVector<TRAITS>::indexOf(DataId stub)
{
  size_t  low = 0, high = dataCount, mid;
  while (low < high) {
    mid = (low + high)/2;
    int res = TRAITS::Compare(stub, data[mid]);
    if (res < 0) {
      high = mid;
    }
    else if (res > 0) {
      low = mid + 1;
    }
    else {
      return mid;
    }
  }
  return NOT_FOUND;
}

template <class TRAITS>
bool  SortedVector<TRAITS>::insertIfNotExists(Data obj)
{
  size_t  low = 0, high = dataCount, mid;
  while (low < high) {
    mid = (low + high)/2;
    int res = TRAITS::Compare(obj, data[mid]);
    if (res < 0) {
      high = mid;
    }
    else if (res > 0) {
      low = mid + 1;
    }
    else {
      return false; //element already exists
    }
  }
  insertAt(low, obj);
  return true;
}

template <class TRAITS>
bool  SortedVector<TRAITS>::removeIfExists(DataId stub)
{
  size_t  idx = indexOf(stub);
  if (idx == NOT_FOUND) {
    return false; //no such element
  }
  else {
    removeAt(idx);
    return true;
  }
}

#endif //UTIL_SORTVEC_H
