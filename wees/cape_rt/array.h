#ifndef CAPE_ARRAY_H
#define CAPE_ARRAY_H

#include <assert.h>
#include <malloc.h>

// A simple std::vector class
// NOTE: won't destroy the contained objects upon destruction
template <class DATA, int INITIAL_CAPACITY=16, int CAPACITY_INCREMENT=16>
class Array {
  DATA*   _data;
  size_t  _size;
  size_t  _capacity;

public:
  Array()
  {
    _size = 0;
    _data = (DATA*) malloc(sizeof(DATA) * (_capacity = INITIAL_CAPACITY));
    assert(_data);
  }

 ~Array()
  {
    free(_data);
#ifdef _DEBUG
    _data = 0;
    _size = 0;
#endif
  }

  // NOTE: elements are not destroyed!
  void  clear()
  {
    _size = 0;
  }

  size_t  size() const
  {
    return _size;
  }

  DATA& operator [] (size_t idx)
  {
    assert(idx < _size);
    return _data[idx];
  }

  const DATA& operator [] (size_t idx) const
  {
    assert(idx < _size);
    return _data[idx];
  }

  void  push_back(const DATA& data)
  {
    if (_size == _capacity) {
      _data = (DATA*) realloc(_data, sizeof(DATA) * (_capacity += CAPACITY_INCREMENT));
      assert(_data);
    }
    _data[_size++] = data;
  }
};

#endif