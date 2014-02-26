#ifndef jacc_spacer_h
#define jacc_spacer_h

#include <iostream.h>
#include "util/assert.h"

struct jacc_spacer_base
{
  protected:
    static const char* space_str_beg;
    static const char* space_str_end;

  public:
    // returns the width of the string representation of num
    // e.g. strwidth(-1997) == 5
    int  strwidth(int num);
};

template <int WIDTH>
struct jacc_spacer : jacc_spacer_base
{
  protected:
    const char* p_spc_slice;

  public:
    jacc_spacer(int n=0)
      : p_spc_slice(space_str_end - n * WIDTH)
      {
        ASSERT(space_str_beg <= p_spc_slice);
        ASSERT(p_spc_slice <= space_str_end);
      }

    jacc_spacer& operator ++ ()
      {
        ASSERT(space_str_beg <= (p_spc_slice - WIDTH));
        p_spc_slice -= WIDTH;
        return *this;
      }

    jacc_spacer& operator -- ()
      {
        ASSERT((p_spc_slice + WIDTH) <= space_str_end);
        p_spc_slice += WIDTH;
        return *this;
      }

    jacc_spacer  operator ++ (int)
      {
        ASSERT(space_str_beg <= (p_spc_slice - WIDTH));
        jacc_spacer res(*this);
        p_spc_slice -= WIDTH;
        return res;
      }

    jacc_spacer  operator -- (int)
      {
        ASSERT((p_spc_slice + WIDTH) <= space_str_end);
        jacc_spacer res(*this);
        p_spc_slice += WIDTH;
        return res;
      }

    const char* pad(int num)
      {
        const char* p_pad_slice = p_spc_slice + strwidth(num);
        return p_pad_slice <= space_str_end ? p_pad_slice : space_str_end;
      }

    const char* c_str() const
      {
        return p_spc_slice;
      }
    
    ostream& put_in(ostream& os) const
      {
        if (1 < WIDTH)
          os << '\n';
        return os << p_spc_slice;
      }        
};

template <int WIDTH>
inline ostream& operator << (ostream& os, const jacc_spacer<WIDTH>& spacer)
  {
    return spacer.put_in(os);
  }

typedef jacc_spacer<1>  jacc_space;
typedef jacc_spacer<2>  jacc_tab;

#endif //jacc_spacer_h
