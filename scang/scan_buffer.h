#ifndef scan_buffer_h
#define scan_buffer_h

#include <assert.h>

class scan_buffer
{
  public:
    typedef unsigned char  char_t;

    struct allocator
    {
      virtual ~allocator() {;}

      // Allocates and returns a new buffer of the specified size.
      virtual char_t* new_buffer(int size);

      // Deallocates an old buffer of the specified size.
      virtual void    del_buffer(char_t* buf, int size);
    };

  private:
    char_t*   _buf;
    char_t*   _get;
    char_t*   _pos;
    char_t*   _eget;
    char_t*   _ebuf;

    int       _mark_len;

    int       _read_stat; // the last value returned by read()
    int       _error_val; // value returned when read() fails

    int       _chars_got; // the total number of chars `flushed' so far

    allocator*_allocator;

  protected:
    char_t*  buf_ptr() const { return _buf;  }
    char_t* ebuf_ptr() const { return _ebuf; }
    char_t*  get_ptr() const { return _get;  }
    char_t* eget_ptr() const { return _eget; }
    char_t*  pos_ptr() const { return _pos;  }

    int     buf_size() const { return _ebuf - _buf; }
    int     get_size() const { return _eget - _get; }

    void    set_chars_got(int ncg) { _chars_got = ncg; }

    // invoked when current position reaches end of the get area
    int   underflow();

    // Do we need a new (bigger) buffer ?
    // Invoked only on buffer underflow.
    // Returns 0 if old buffer is ok, or the size of the new buffer to allocate.
    virtual int  needs_new_buffer();

    // Does the actual input.
    // This method may return only values <= nmax_read.
    // If the value returned is
    // 0 < N <= max_read then N chars were successfully input and stored.
    // N = 0 then the input is (possibly temporally) blocked but later attempts
    //       may succeed. Further invokations of read(...) are possible.
    //       In this particular case the value returned from underflow()
    //       (and thus get()) will be the value of _error_val.
    // N < 0 then EOF or some abnormal situation was encountered. In this
    //       case there will be no further invokations of read(...).
    virtual int   read(char_t* pstore, int nmax_read) const =0;

  public:
    // returns next character input OR error code (< 0) OR zero_subst
    int     get();

    const char_t* mark_beg() const { return _get; }
    const char_t* mark_end() const { return _get + _mark_len; }
    int           mark_len() const { return _mark_len; }
    int           mark_beg_charno() const { return _chars_got; }
    int           mark_end_charno() const { return _chars_got + _mark_len; }

    // sets marker to current _pos
    void  mark();

    int   error_val() const;
    void  set_error_val(int ev);
    void  set_allocator(allocator* alloc);

    // flushes the [_get, _get + _mark_len) part of the get area and
    // resets current _pos to _get and _mark_len to 0
    void  flush_to_mark();
    // resets current _pos to (_get + _mark_len)
    void  reset_to_mark();

              scan_buffer();

    virtual  ~scan_buffer();
};

inline
void  scan_buffer::mark()
{
  _mark_len = _pos - _get;
}

inline
int   scan_buffer::get()
{
  return (_pos < _eget ? *_pos++ : underflow());
}

inline
void  scan_buffer::flush_to_mark()
{
  _pos = _get += _mark_len;
  _chars_got  += _mark_len;
  _mark_len = 0;
}

inline
void  scan_buffer::reset_to_mark()
{
  _pos = _get + _mark_len;
}

inline
int   scan_buffer::error_val() const
{
  return _error_val;
}

inline
void  scan_buffer::set_error_val(int zs)
{
  // assert((char_t)zs != zs);
  _error_val = zs;
}

inline
void  scan_buffer::set_allocator(allocator* alloc)
{
  assert(alloc != 0);
  assert(_buf == 0);
  delete _allocator;
  _allocator = alloc;
}

#endif
