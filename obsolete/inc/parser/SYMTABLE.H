#ifndef PARSER_SYMTABLE_H
#define PARSER_SYMTABLE_H

#include "util/hashtab.h"
#include "util/string.h"
#include "parser/tt.h"
#include "parser/debug.h"


struct Symbol
{
  TT    keyword;
  char  strid[1];
};

typedef const char* symbol_t;

extern  symbol_t  TheStubSymbol;

inline
bool  isKeyword(symbol_t sym)
{
  return ((Symbol*)(sym - offsetof(Symbol, strid)))->keyword != TT_NOTOKEN;
}

inline
TT  toKeyword(symbol_t sym)
{
  PARSER_ASSERT(isKeyword(sym));
  return ((Symbol*)(sym - offsetof(Symbol, strid)))->keyword;
}


class SymbolHashTraits
{
  public:
    class   HashTableBase {};

    typedef Symbol*     Data;    //the data type stored in hashtable entries
    typedef Symbol*     DataIn;  //the param type of function put
    typedef Symbol*     DataOut; //the return type of function get
    struct  Key {                //the param type of function get
      const char* strid;
      size_t      len;
    };

    static  bool    Equals(const Key& key, Symbol* sym)
      {
        return 0 == strncmp(key.strid, sym->strid, key.len);
      }

    static  hash_t  Hash(Symbol* sym)
      {
        return string::Hash(sym->strid, strlen(sym->strid));
      }

    static  hash_t  Hash(const Key& key)
      {
        return string::Hash(key.strid, key.len);
      }

    static  Symbol* NullData()
      {
        return 0;
      }
    static  void    FlushData(Symbol* sym, bool ownsData)
      {
        if (ownsData) {
          delete[] (char*)sym;
        }
      }
};


class SymbolTable
{
  private:
    HashTable<SymbolHashTraits> table;
  public:
    symbol_t  getSymbol(const char* s, size_t l);
};

#endif //PARSER_SYMTABLE_H
