#ifndef UTIL_LEXER_H
#define UTIL_LEXER_H

#include "util/lincol.h"
#include "util/string.h"

#ifndef LEXER_CHAR_T_DEFINED
typedef  char           char_t;
#endif
#ifndef LEXER_INT_T_DEFINED
typedef  unsigned long  int_t;
#endif
#ifndef LEXER_FLOAT_T_DEFINED
typedef  double         float_t;
#endif
#ifndef LEXER_TOKEN_T_DEFINED
enum toktag_t {
  TT_NOTOKEN,
  TT_EOF,
  TT_CHAR,
  TT_INT,
  TT_FLOAT,
  TT_SYMBOL,
  TT_STRING
};
#endif
#ifndef LEXER_ERROR_T_DEFINED
typedef  int  error_t;
enum  {
  ErrNone,
  LErr_UnexpectedEOF,
  LErr_IntConstOutOfRange,
  LErr_LongConstOutOfRange,
  LErr_FloatConstOutOfPrecision,
  LErr_DoubleConstOutOfPrecision,
  LErr_BadNumber,
  LErr_BadHexNumber,
  LErr_BadOctNumber,
  LErr_BadFloatNumber,
  LErr_CommentNotClosed,
  LErr_UnterminatedString,
  LErr_UnterminatedChar,
  LErr_BadHexChar,
  LErr_BadUniChar,
  LErr_BadEscapeChar,
  LErr_IllegalCharacter,
};
#endif
//----------------------------------------------------------------------
#ifndef LEXER_ERRHAN_DEFINED
class LexerErrHan
{
  private:
    ostream&  os;
  public:
    unsigned  nerrors;

    LexerErrHan(ostream& os) : os(os), nerrors(0) {}
    void      error(const LinCol& lc, error_t err);
    unsigned  nErrors() const { return nerrors; }
    virtual  const char* getErrorMessage(error_t errcode);
};
#endif
//----------------------------------------------------------------------
#ifndef LEXER_SOURCE_DEFINED

#include "util/vector.h"
#include "util/vectort.h"

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
#endif
//----------------------------------------------------------------------
#ifndef LEXER_SYMTAB_DEFINED

#include "util/hashtab.h"

struct LexerSymbol
{
  toktag_t  keyword;
  char      strid[1];
};

typedef const char* symbol_t;

extern  symbol_t  TheStubSymbol;

inline
bool  isKeyword(symbol_t sym)
{
  return ((LexerSymbol*)(sym - offsetof(LexerSymbol, strid)))
              ->keyword != TT_NOTOKEN;
}

inline
toktag_t  toKeyword(symbol_t sym)
{
  UTIL_ASSERT(isKeyword(sym));
  return ((LexerSymbol*)(sym - offsetof(LexerSymbol, strid)))
              ->keyword;
}


class LexerSymbolHashTraits
{
  public:
    class   HashTableBase {};

    typedef LexerSymbol* Data;    //the data type stored in hashtable entries
    typedef LexerSymbol* DataIn;  //the param type of function put
    typedef LexerSymbol* DataOut; //the return type of function get
    struct  Key {                 //the param type of function get
      const char* strid;
      size_t      len;
    };

    static  bool    Equals(const Key& key, LexerSymbol* sym)
      {
        return 0 == strncmp(key.strid, sym->strid, key.len);
      }

    static  hash_t  Hash(LexerSymbol* sym)
      {
        return string::Hash(sym->strid, strlen(sym->strid));
      }

    static  hash_t  Hash(const Key& key)
      {
        return string::Hash(key.strid, key.len);
      }

    static  LexerSymbol* NullData()
      {
        return 0;
      }
    static  void    FlushData(LexerSymbol* sym, bool ownsData)
      {
        if (ownsData) {
          delete[] (char*)sym;
        }
      }
};


class LexerSymTab
{
  private:
    HashTable<LexerSymbolHashTraits> table;
  public:
    symbol_t  getSymbol(const char* s, size_t l);
};
#endif//LEXER_SYMTAB_DEFINED
//----------------------------------------------------------------------
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                            class Lexan
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
class Lexer
{
  public:
                  Lexer();

    toktag_t      nextToken();

    symbol_t      valueSymbol()   const;
    char_t        valueChar()     const;
    int_t         valueInt()      const;
    float_t       valueFloat()    const;
    string        valueString()   const;
    LinCol        ttBegLC()       const;
    LinCol        ttEndLC()       const;
    const char*   ttStrBeg()      const;
    size_t        ttStrLen()      const;

    LexerSource*  source;
    LexerSymTab*  symbolTable;
    LexerErrHan*  errorHandler;

  protected:

    const char*   sp;
    const char*   cp;
    string        value_string;
    toktag_t      toktag;
  union
  {
    symbol_t      value_symbol;
    char_t        value_char;
    int_t         value_int;
    float_t       value_float;
  };

    toktag_t      readString();    //reads string: "\nHello"
    toktag_t      readChar();      //reads char: 'a','\a','\xff', '\uffff' etc.
    toktag_t      readSymbol();    //reads identifier: a_1 or keyword: for.
    toktag_t      readNumber();    //reads integer: 12 = 0xC = 0n1100
                                   //   or float: 12.03 = 1.203e2; .01 = 1e-2
    bool  scanChar(char& );        //used by readString & readChar.
    bool  skipCComment();          //skips a C style comment: /* ... */
    bool  nextLine();

    void  error(error_t err);
};

#endif//UTIL_LEXER_H
