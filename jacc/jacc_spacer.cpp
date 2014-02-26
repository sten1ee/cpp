#include "jacc_spacer.h"
#include <string.h>

const char*
jacc_spacer_base::space_str_beg = // the next line contains a long space string:
"                                                                         ";
const char*
jacc_spacer_base::space_str_end = space_str_beg + strlen(space_str_beg);

int
jacc_spacer_base::strwidth(int num)
{
  int res = 1;
  if (num < 0) { num = -num; res++; }
  while (0 != (num /= 10))
    res++;
  return res;
}
