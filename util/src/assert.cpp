#include "util/assert.h"
#ifdef DEBUG
#include <stdio.h>

void  __AssertFail(const char* file, unsigned line)
#ifndef _MSC_VER
  throw (__Assert)
#endif
{
  static bool yes_all = false;
  if (yes_all)
    return;

  const char* msg =
"\n\
**************************************\n\
Assertion failure:\n\
\tfile %s line %d\n\
Continue (y/a/n) ?\n";
  fprintf(stderr, msg, file, line);

  for (;;) {
    switch (getchar()) {
      case 'a': yes_all = true;
      case 'y': return;
      case EOF:
      case 'n': throw __Assert(file, line);
    }
  }
}

#endif //defined(DEBUG)
