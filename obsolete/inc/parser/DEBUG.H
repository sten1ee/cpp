#ifndef PARSER_DEBUG_H
#define PARSER_DEBUG_H

#if !defined(PARSER_DEBUG) && !defined(PARSER_NDEBUG)
#    define  PARSER_DEBUG
#endif

#if  defined(PARSER_DEBUG)
#    include "util/assert.h"
#    define  PARSER_ASSERT(COND) ASSERT(COND)
#    define  PARSER_DEBUGIN(STM) STM
#else
#    define  PARSER_ASSERT(COND)
#    define  PARSER_DEBUGIN(STM)
#endif

#endif
