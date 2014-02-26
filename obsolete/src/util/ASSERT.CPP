#include "util/assert.h"
#include <iostream.h>

void* __debug_ptr;

void  __AssertionFailure(const char* file, unsigned line)
{
  cout.flush();
  cout
  << endl
  << "**************************************\n"
  << "Assertion failure:\n"
  << "\tfile " << file << " line " << line << '\n'
  << "Continue (y/n) ?"
  << endl;
  char ch;
  for (;;) {
    cin >> ch;
    if (ch == 'y')
      break;
    if (ch == 'n')
      throw FailedAssert(file, line);
  }
}

