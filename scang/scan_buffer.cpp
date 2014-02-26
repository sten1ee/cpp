#include "scan_buffer.h"
#include <mem.h>


scan_buffer::char_t* scan_buffer::allocator::new_buffer(int size)
{
  return new char_t[size];
}


void  scan_buffer::allocator::del_buffer(char_t* buf, int /*size*/)
{
  delete[] buf;
}


int  scan_buffer::needs_new_buffer()
{
  int buf_sz = buf_size();
  int get_sz = get_size();
  int free_sz =  buf_sz - get_sz;
  if (16 <= free_sz)
    return 0;
  else
    return (buf_sz ? 2 * buf_sz : 256);
}


scan_buffer::scan_buffer()
{
  _allocator = new allocator();
  _buf = _ebuf = _get = _eget = _pos = 0;
  _mark_len = _read_stat = _chars_got = 0;
  set_error_val(-1);
}


scan_buffer::~scan_buffer()
{
  if (_buf) {
    _allocator->del_buffer(_buf, buf_size());
  }
  delete _allocator;
}


int   scan_buffer::underflow()
{
  assert(_pos == _eget);

  if (_read_stat < 0) {
    return _error_val;
  }

  if (_eget == _ebuf) {
    char_t* old_buf  = 0;
    int new_size = needs_new_buffer();
    if (new_size) {
      assert(get_size() < new_size);
      old_buf = _buf;
      _buf = _allocator->new_buffer(new_size);
    }

    // move the `get area' to the base:
    int get_size = _eget - _get;
    if (get_size != 0) {
      memmove(_buf, _get, get_size);
    }
    _pos = _eget = (_get = _buf) + get_size;

    if (new_size) {
      _allocator->del_buffer(old_buf, _ebuf - old_buf);
      _ebuf = _buf + new_size;
    }
  }

  _read_stat = read(_eget, _ebuf - _eget);
  if (0 < _read_stat) {
    assert(_eget + _read_stat <= _ebuf);
    _eget += _read_stat;
    return *_pos++;
  }
  else {
    return _error_val;
  }
}

