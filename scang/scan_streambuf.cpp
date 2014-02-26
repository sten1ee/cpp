#ifndef scan_streambuf_h
#define scan_streambuf_h

template <class STREAMBUF>
class scan_streambuf : public STREAMBUF
{
  private:
    typedef STREAMBUF parent;

    int       _mark_len;
    int       _chars_got; // the total number of chars `flushed' so far

  public:

    // returns next character input OR error code (< 0) OR zero_subst
    //int     get() { return sbumpc(); }
/*
    const char_t* mark_beg() const { return eback(); }
    const char_t* mark_end() const { return eback() + _mark_len; }
    int           mark_len() const { return _mark_len; }
    int           mark_beg_charno() const { return _chars_got; }
    int           mark_end_charno() const { return _chars_got + _mark_len; }
*/
    // sets marker to current _pos
    void  mark() { _mark_len = gptr() - eback(); }

    // flushes the [_get, _get + _mark_len) part of the get area and
    // resets current _pos to _get and _mark_len to 0
    void  flush_to_mark();
    // resets current _pos to (_get + _mark_len)
    void  reset_to_mark();

};

template <class STREAMBUF>
inline
void  scan_streambuf<STREAMBUF>::flush_to_mark()
{
  setg(eback() + _mark_len, eback() + _mark_len, egptr());
  _chars_got  += _mark_len;
  _mark_len = 0;
}

template <class STREAMBUF>
inline
void  scan_streambuf<STREAMBUF>::reset_to_mark()
{
  gbump(_mark_len - (gptr() - eback()));
}


#endif
#include <fstream.h>

scan_streambuf<filebuf> scan_filebuf;
