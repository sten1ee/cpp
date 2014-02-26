#ifndef UTIL_TYPES_H
#define UTIL_TYPES_H

#include <stddef.h>
#include <limits.h>


#define SIZET_MAX (size_t(ULONG_MAX))

#ifdef OPTION_NO_BOOL
# define bool  int
# define false 0
# define true  1
#endif

const bool    OWNS_DATA = true;
const bool    DEL_DATA  = true;

#ifdef OPTION_NO_TYPENAME
# define typename
#endif

#if   UINT_MAX == 0xffffffffUL
typedef unsigned      uint32;
typedef int            int32;
# define UINT32_MAX   UINT_MAX
# define  INT32_MAX    INT_MAX
# define  INT32_MIN    INT_MIN
#elif ULONG_MAX == 0xffffffffUL
typedef unsigned long uint32;
typedef long           int32;
# define UINT32_MAX   ULONG_MAX
# define  INT32_MAX    LONG_MAX
# define  INT32_MIN    LONG_MIN
#else
# error "can't match the exact 32-bit int type"
#endif

typedef unsigned long hash_t;

#endif
