#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <stddef.h>

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif

const int     NO_AUTO_INCREMENT = -1;
const int     DOUBLE_INCREMENT  =  0;
const int     DEFAULT_CAPACITY_INCREMENT = DOUBLE_INCREMENT;
const size_t  DEFAULT_INITIAL_CAPACITY   = 16;

template <class TRAITS> class Vector : public TRAITS::VectorBase
{
  public:
    typedef TRAITS   Traits;
    typedef typename Traits::Data        Data;
    typedef typename Traits::DataIn      DataIn;
    typedef typename Traits::DataOut     DataOut;

  protected:
      Data*   data;
      size_t  dataCount;
      bool    ownsDat;
      size_t  capacity;
      int     capacityIncrement;

    void  init(size_t initCap, int capInc, bool ownsData)
      {
        data              = Traits::AllocDataBuf(initCap);
        dataCount         = 0;
        capacity          = initCap;
        capacityIncrement = capInc;
        ownsDat           = ownsData;
      }

    void  relinquish()
      {
        data      = 0;
        dataCount = 0;
        capacity  = 0;
      }

    void  copy(Vector& v)
      {
        data              = v.data;
        dataCount         = v.dataCount;
        capacity          = v.capacity;
        capacityIncrement = v.capacityIncrement;
        ownsDat           = v.ownsDat;
      }

    void  occupy(Vector& v)
      {
        copy(v);
        v.relinquish();
      }
/*
    Vector()
      {
        data              = 0;
        dataCount         = 0;
        capacity          = 0;
        capacityIncrement = 0;
        ownsDat           = false;
      }
*/
  public:
    Vector(bool ownsData=true)
      {
        //try prevent things like: Vector(vectorSize);
        UTIL_ASSERT(int(ownsData) == 0 || int(ownsData) == 1);
        init(DEFAULT_INITIAL_CAPACITY, DEFAULT_CAPACITY_INCREMENT, ownsData);
      }

    Vector(size_t initialCapacity, int capacityIncrement)
      {
        init(initialCapacity, capacityIncrement, OWNS_DATA);
      }

    Vector(Vector& v)
      {
        occupy(v);
      }

   ~Vector()
      {
        Traits::FlushDataBuf(data, dataCount, ownsData());
        Traits::FreeDataBuf(data, capacity);
      }

    Vector&  assignGut(Vector& v)
      {
        Traits::FlushDataBuf(data, dataCount, ownsData());
        Traits::FreeDataBuf(data, capacity);
        occupy(v);
        return *this;
      }

#ifdef OPTION_NO_NONCONST_ASSIGN
    Vector&  operator = (Vector  v)
#else
    Vector&  operator = (Vector& v)
#endif
      {
        return  assignGut(v);
      }

    void    trimToSize()            { setCapacity(dataCount); }
    size_t  getCapacity()     const { return capacity;        }
    size_t  size()            const { return dataCount;       }
    bool    isEmpty()         const { return dataCount == 0;  }
    bool    ownsData()        const { return ownsDat;         }
    void    ownsData(bool ownsData) { ownsDat = ownsData;     }
    void    ensureInsertion();
    void    setCapacity(size_t newCapacity);

    DataOut operator [] (size_t index) const
      {
        UTIL_ASSERT(index < dataCount);
        return data[index];
      }

    Data& operator [] (size_t index)
      {
        UTIL_ASSERT(index < dataCount);
        return data[index];
      }

    void removeAt(size_t index, bool delData)
      {
        UTIL_ASSERT(index < dataCount);
        Traits::RemoveEntry(data[index],
                          --dataCount - index,
                            delData);
      }

    void removeAt(size_t index)
      {
        removeAt(index, ownsData());
      }

    void insertAt(size_t index, DataIn elem)
      {
        UTIL_ASSERT(index <= dataCount);
        ensureInsertion();
        Traits::InsertEntry(data[index],
                            dataCount++ - index,
                            elem);
      }

    void push(DataIn elem)
      {
        ensureInsertion();
        Traits::PushEntry(data[dataCount++], elem);
      }

    Data pop()
      {
        UTIL_ASSERT(dataCount > 0);
        return Traits::PopEntry(data[--dataCount]);
      }

    DataOut top() const
      {
        UTIL_ASSERT(dataCount > 0);
        return data[dataCount-1];
      }

    Data& top()
      {
        UTIL_ASSERT(dataCount > 0);
        return data[dataCount-1];
      }

    void flush(bool ownsData)
      {
        Traits::FlushDataBuf(data, dataCount, ownsData);
        dataCount = 0;
      }

    void flush()
      {
        flush(ownsData());
      }
};


template <class TRAITS>
void Vector<TRAITS>::setCapacity(size_t newCapacity)
{
  UTIL_ASSERT(dataCount <= newCapacity);
  if (newCapacity != capacity) {
    data     = Traits::ReallocDataBuf(data, dataCount, newCapacity);
    capacity = newCapacity;
  }
}

template <class TRAITS>
void Vector<TRAITS>::ensureInsertion()
{
  if (dataCount == capacity) {
    UTIL_ASSERT(0 <= capacityIncrement); // non-auto resizable vector!
    setCapacity((capacityIncrement > 0) ? (capacity + capacityIncrement)
                                        : (capacity * 2));
    UTIL_ASSERT(dataCount < capacity);
  }
}

#endif //UTIL_VECTOR_H
