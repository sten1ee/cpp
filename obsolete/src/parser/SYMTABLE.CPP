#include "parser/symtable.h"

Symbol TheStubSymbolStruct = { TT_NOTOKEN, { 0 } };

symbol_t TheStubSymbol = TheStubSymbolStruct.strid;

symbol_t  SymbolTable::getSymbol(const char* s, size_t l)
{
  SymbolHashTraits::Key key = { s, l };
  Symbol* sym = table.get(key);
  if (!sym) {
    sym = (Symbol*)new char[sizeof(Symbol) + l];
    sym->keyword = TT_NOTOKEN;
    strncpy(sym->strid, s, l);
    sym->strid[l] = 0;
    table.put(sym);
  }
  return sym->strid;
}

