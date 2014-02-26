#ifndef PARSER_LEX_H
#define PARSER_LEX_H

#include <iostream.h>
#include "util/wstring.h"
#include "util/hugeint.h"
#include "parser/source.h"
#include "parser/symtable.h"
#include "parser/tt.h"
#include "parser/error.h"
#include "parser/long.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                              class Lex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
class Lex
{
  public:
                  Lex();

    TT            nextToken();

    symbol_t      valueSymbol()   const;
    wchar_t       valueChar()     const;
    int32         valueInt()      const;
    Long          valueLong()     const;
    float         valueFloat()    const;
    double        valueDouble()   const;
    wstring       valueString()   const;
    LinCol        ttBegLC()       const;
    LinCol        ttEndLC()       const;
    const char*   ttStrBeg()      const;
    size_t        ttStrLen()      const;

    Source*       source;
    SymbolTable*  symbolTable;
    ErrorHandler* errorHandler;

  protected:
  PARSER_DEBUGIN(
    TT            tokenType;
  )
    const char*   sp;
    const char*   cp;
    wstring       value_string;
  union
  {
    symbol_t      value_symbol;
    wchar_t       value_char;
    int32         value_int;
    Long          value_long;
    float         value_float;
    double        value_double;
  };

    TT    readString();    //reads string: "\nHello"
    TT    readChar();      //reads char: 'a','\a','\xff', '\uffff' etc.
    TT    readSymbol();    //reads identifier: a_1 or keyword: for.
    TT    readNumber();    //reads integer: 12 = 0xC = 0n1100
                           //   or float: 12.03 = 1.203e2; .01 = 1e-2
    bool  scanChar(wchar_t& );  //used by readString & readChar.
    bool  skipCComment();       //skips a C style comment: /* ... */
    bool  nextLine();

    void  error(error_t err);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                  inline members of Lex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline
Lex::Lex()
  : sp(0),
    cp(""),
    source(0),
    symbolTable(0),
    errorHandler(0)
  {}

inline
symbol_t      Lex::valueSymbol()   const
  {
    PARSER_ASSERT(tokenType == TT_Symbol);
    return value_symbol;
  }

inline
wchar_t       Lex::valueChar()     const
  {
    PARSER_ASSERT(tokenType == TT_Char);
    return value_char;
  }

inline
int32         Lex::valueInt() const
  {
    PARSER_ASSERT(tokenType == TT_Int);
    return value_int;
  }

inline
Long          Lex::valueLong() const
  {
    PARSER_ASSERT(tokenType == TT_Long);
    return value_long;
  }

inline
float         Lex::valueFloat()   const
  {
    PARSER_ASSERT(tokenType == TT_Float);
    return value_float;
  }

inline
double        Lex::valueDouble()   const
  {
    PARSER_ASSERT(tokenType == TT_Double);
    return value_double;
  }

inline
wstring       Lex::valueString()   const
  {
    PARSER_ASSERT(tokenType == TT_String);
    return value_string;
  }

inline
LinCol        Lex::ttBegLC()       const
  {
    return source->pos(sp);
  }

inline
LinCol        Lex::ttEndLC()       const
  {
    return source->pos(cp);
  }

inline
const char*   Lex::ttStrBeg()      const
  {
    return sp;
  }

inline
size_t        Lex::ttStrLen()      const
  {
    return size_t(cp - sp);
  }

#endif //PARSER_LEX_H
