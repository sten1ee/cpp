#ifndef scanner_h
#define scanner_h

#ifndef lr_symbol_h
# include "lr_symbol.h"
#endif

/** Abstract class (interface) for the scanner used by lr_parser */
class scanner
{
  public:
             scanner() {;}
    virtual ~scanner() {;}

    virtual  lr_symbol*   next_token()=0;
};

#endif //scanner_h
