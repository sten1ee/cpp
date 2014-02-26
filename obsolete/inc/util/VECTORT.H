#ifndef UTIL_VECTORT_H
#define UTIL_VECTORT_H

#include <string.h>

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif

#pragma intrisinc memcpy
#pragma intrisinc memmove

enum  VectorTraitsTag{};
enum  VectorPtrTraitsTag{};

template <class DATA> class VectorTraits
{
  public:
    class                     VectorBase {};
    typedef DATA              Data;
    typedef DATA              DataIn;
    typedef DATA              DataOut;
    typedef DATA              DataId;
    typedef VectorTraitsTag   Tag;
//------------------------------------------------------------------------------
    static  Data* AllocDataBuf(size_t capacity)
      {
        return new Data[capacity];
      }

    static  Data* ReallocDataBuf(Data* data, size_t dataCount, size_t newCapacity)
      {
        Data* newData = new Data[newCapacity];
        memcpy(newData, data, sizeof(Data)*dataCount);
        delete[] data;
        return newData;
      }

    static  void  FreeDataBuf(Data* data, size_t /*capacity*/)
      {
        delete[] data;
      }

    static  void  FlushDataBuf(Data* /*data*/, size_t /*dataCount*/, bool /*ownsData*/)
      {}

//------------------------------------------------------------------------------
    static  void  RemoveEntry(Data& entry, size_t upperCount, bool /*ownsData*/)
      {
        memmove(&entry, &entry + 1, sizeof(Data)*upperCount);
      }

    static  void  InsertEntry(Data& entry, size_t upperCount, DataIn elem)
      {
        memmove(&entry + 1, &entry, sizeof(Data)*upperCount);
        entry = elem;
      }

    static  Data  PopEntry(Data& entry)
      {
        return entry;
      }

    static  void  PushEntry(Data& entry, DataIn elem)
      {
        entry = elem;
      }
//------------------------------------------------------------------------------
};


template <class DATA> class VectorPtrTraits : public VectorTraits<void*>
{
  public:
    typedef DATA*               Data;
    typedef DATA*               DataIn;
    typedef DATA*               DataOut;
    typedef DATA*               DataId;
    typedef VectorPtrTraitsTag  Tag;
    typedef VectorTraits<void*> Parent;

//------------------------------------------------------------------------------
    static  Data* AllocDataBuf(size_t capacity)
      {
        return (Data*)Parent::AllocDataBuf(capacity);
      }

    static  Data* ReallocDataBuf(Data* data, size_t dataCount, size_t newCapacity)
      {
        return (Data*)Parent::ReallocDataBuf((void**)data, dataCount, newCapacity);
      }

    static  void  FreeDataBuf(Data* data, size_t capacity)
      {
        Parent::FreeDataBuf((void**)data, capacity);
      }

    static  void  FlushDataBuf(Data* data, size_t dataCount, bool ownsData);

//------------------------------------------------------------------------------
    static  void  RemoveEntry(Data& entry, size_t upperCount, bool ownsData)
      {
        if (ownsData)
          delete entry;
        memmove(&entry, &entry + 1, sizeof(Data)*upperCount);
      }

    static  void  InsertEntry(Data& entry, size_t upperCount, DataIn elem)
      {
        memmove(&entry + 1, &entry, sizeof(Data)*upperCount);
        entry = elem;
      }

    static  Data  PopEntry(Data& entry)
      {
        return entry;
      }

    static  void  PushEntry(Data& entry, DataIn elem)
      {
        entry = elem;
      }
//------------------------------------------------------------------------------
};

template <class DATA>
void  VectorPtrTraits<DATA>::FlushDataBuf(Data* data, size_t dataCount, bool ownsData)
{
  if (ownsData)
    for (size_t i = dataCount; 0 < i--; )
      delete data[i];
}

#endif //UTIL_VECTORT_H
