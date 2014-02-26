#ifndef UTIL_HUGEINT_H
#define UTIL_HUGEINT_H

#include <math.h>

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif

#ifdef OPTION_NO_LD_LIBROUTINES
long double  ldexpl(long double x, int exp);
long double  modfl (long double x, long double* ipart);
#endif

class HugeInt
{
  protected:
    long double n;

    HugeInt(long double _n)
      {
        n = _n;
      }

  public:
    static const HugeInt HugeInt::UINT64_MAX;
    static const HugeInt HugeInt:: INT64_MIN;
    static const HugeInt HugeInt:: INT64_MAX;

    static const HugeInt HugeInt::POW_2_64; //2 raised to the power of 64

    HugeInt()
      {
      }

    HugeInt(const HugeInt& hugeInt)
      {
        n = hugeInt.n;
      }

    HugeInt(int i)
      {
        n = i;
      }

    HugeInt(unsigned u)
      {
        n = u;
      }

    HugeInt(long l)
      {
        n = l;
      }

    HugeInt(unsigned long ul)
      {
        n = ul;
      }

    static  HugeInt   Make_uint64(uint32  high, uint32  low)
      {
        return HugeInt(ldexpl(high, 32) + low);
      }

    static  HugeInt   Make_int64(uint32  high, uint32  low)
      {
        long double n = ldexpl(high, 32) + low;
        if (INT32_MAX < high) {
          n -= POW_2_64.n;
        }
        return HugeInt(n);
      }

    HugeInt& operator = (const HugeInt& hugeInt)
      {
        n = hugeInt.n;
        return *this;
      }

    bool    operator ==(const HugeInt& hugeInt) const
      {
        return n == hugeInt.n;
      }

    bool    operator !=(const HugeInt& hugeInt) const
      {
        return n != hugeInt.n;
      }

    bool    operator < (const HugeInt& hugeInt) const
      {
        return n < hugeInt.n;
      }

    bool    operator <=(const HugeInt& hugeInt) const
      {
        return n <= hugeInt.n;
      }

    HugeInt  operator - () const
      {
        return HugeInt(-n);
      }

    HugeInt  operator + (const HugeInt& hugeInt) const
      {
        return HugeInt(n + hugeInt.n);
      }

    HugeInt  operator - (const HugeInt& hugeInt) const
      {
        return HugeInt(n - hugeInt.n);
      }

    HugeInt  operator * (const HugeInt& hugeInt) const
      {
        return HugeInt(n * hugeInt.n);
      }

    HugeInt& operator +=(const HugeInt& hugeInt)
      {
        n += hugeInt.n;
        return *this;
      }

    HugeInt& operator -=(const HugeInt& hugeInt)
      {
        n -= hugeInt.n;
        return *this;
      }

    HugeInt& operator *=(const HugeInt& hugeInt)
      {
        n *= hugeInt.n;
        return *this;
      }

    bool  is_uint64() const
      {
        return 0 <= n && n <= UINT64_MAX.n;
      }

    bool  is_int64() const
      {
        return INT64_MIN.n <= n && n <= INT64_MAX.n;
      }

    uint32  uint64_high() const
      {
        UTIL_ASSERT(is_uint64());
        long double ipart;
        modfl(ldexpl(n, -32), &ipart);
        return (uint32)ipart;
      }

    uint32  uint64_low() const
      {
        UTIL_ASSERT(is_uint64());
        long double ipart;
        return (uint32)ldexpl(modfl(ldexpl(n, -32), &ipart), 32);
      }

    uint32  int64_high() const
      {
        UTIL_ASSERT(is_int64());
        if (0 <= n)
          return  uint64_high();
        else
          return  (POW_2_64 + *this).uint64_high();
      }

    uint32  int64_low() const
      {
        UTIL_ASSERT(is_int64());
        if (0 <= n)
          return  uint64_low();
        else
          return  (POW_2_64 + *this).uint64_low();
      }

    long double to_long_double() const
      {
        return n;
      }
};


#endif //UTIL_HUGEINT_H
