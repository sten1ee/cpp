#include "util/hugeint.h"


const HugeInt HugeInt::UINT64_MAX = Make_uint64(0xffffffffUL, 0xffffffffUL);
const HugeInt HugeInt:: INT64_MAX = Make_int64 (0x7fffffffUL, 0xffffffffUL);
const HugeInt HugeInt:: INT64_MIN = Make_int64 (0x80000000UL, 0x00000000UL);


const HugeInt HugeInt::POW_2_64(ldexpl(1, 64)); //2 raised to the power of 64

#ifdef OPTION_NO_LD_LIBROUTINES

long double  ldexpl(long double x, int exp)
{
  long double d;
  if (exp < 0) {
    exp = -exp;
    d = 1.0/2.0;
  }
  else {
    d = 2.0;
  }

  while (exp--) {
    x *= d;
  }
  return x;
}

long double  modfl (long double x, long double* ipart)
{
  long double i;// = (lo
  return 0.0;
}

#endif

