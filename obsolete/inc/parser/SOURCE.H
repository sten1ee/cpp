#ifndef  PARSER_SOURCE_H
#define  PARSER_SOURCE_H

#include "util/vector.h"
#include "util/vectort.h"
#include "parser/lincol.h"

class iostream;

class Source
{
  protected:
    istream&                        is;
    unsigned long                   lineno;
    Vector< VectorTraits< char > >  line;

                  Source();
  public:
                  Source(istream& is);

    const char*   nextLine();
    LinCol        pos(const char* pc);
};

#endif //PARSER_SOURCE_H
