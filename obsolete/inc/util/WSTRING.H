#ifndef UTIL_WSTRING_H
#define UTIL_WSTRING_H

#ifndef   UTIL_STRING_H
#include "util/string.h"
#endif

size_t    strlen(const wchar_t* wstr);

typedef   String<wchar_t>  wstring;

#endif //UTIL_WSTRING_H
