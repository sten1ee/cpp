#ifndef  UTIL_LEXERSRC_H
#define  UTIL_LEXERSRC_H

#include "util/vector.h"
#include "util/vectort.h"
#include "util/lincol.h"

#include <iostream.h>

class LexerSource
{
  protected:
    istream&                        is;
    unsigned long                   lineno;
    Vector< VectorTraits< char > >  line;

                  LexerSource();
  public:
                  LexerSource(istream& is);

    const char*   nextLine();
    LinCol        pos(const char* pc);
};

#endif //UTIL_LEXERSRC_H
