#include <stddef.h>

size_t  strlen(const wchar_t* wstr)
{
  const wchar_t* p = wstr;
  while (*p != (wchar_t)0)
    p++;
  return size_t(p - wstr);
}
