#ifndef UTIL_DEBUG_H
#define UTIL_DEBUG_H

#if !defined(UTIL_DEBUG) && !defined(UTIL_NDEBUG)
#    define  UTIL_DEBUG
#endif

#if  defined(UTIL_DEBUG)
#    include "util/assert.h"
#    define  UTIL_ASSERT(COND) ASSERT(COND)
#    define  UTIL_DEBUGIN(STM) DEBUGIN(STM)
#    define  SAFE_CAST(POBJ, CLASS) (dynamic_cast<CLASS*>(POBJ))
#    define  SURE_CAST(POBJ, CLASS) (ASSERT_NZ(dynamic_cast<CLASS*>(POBJ)))

#else
#    define  UTIL_ASSERT(COND)
#    define  UTIL_DEBUGIN(STM)
#    define  SAFE_CAST(POBJ, CLASS) dynamic_cast<CLASS*>(POBJ)
#    define  SURE_CAST(POBJ, CLASS) static_cast<CLASS*>(POBJ)
#endif

#endif
