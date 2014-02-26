//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parser/syn.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include  "parser/syn.h"


int   SYN::Syn::ReadModifs()
{
  int modifs = 0, mod = 0;
  while (true) {
    switch (tt) {
      case TT_public       : mod = M_PUBLIC;        break;
      case TT_private      : mod = M_PRIVATE;       break;
      case TT_protected    : mod = M_PROTECTED;     break;
      case TT_static       : mod = M_STATIC;        break;
      case TT_final        : mod = M_FINAL;         break;
      case TT_synchronized : mod = M_SYNCHRONIZED;  break;
      case TT_threadsafe   : mod = M_THREADSAFE;    break;
      case TT_transient    : mod = M_TRANSIENT;     break;
      case TT_native       : mod = M_NATIVE;        break;
      case TT_abstract     : mod = M_ABSTRACT;      break;
      default              : return modifs;
    }
    if (mod & modifs) {
      error(Err_DuplicateModifier);
    }
    modifs |= mod;
    nextToken();
  }
}


bool  SYN::Syn::IsPrimeType(TT tt)
{
  return (tt & TT_PRIME_TYPE) == TT_PRIME_TYPE;
}


bool  SYN::Syn::CanStartLocalDecl(TT tt)
{
  return (tt & TT_PRIME_TYPE) == TT_PRIME_TYPE ||
         (tt & TT_LOCAL_MODIF)== TT_LOCAL_MODIF||
          tt ==TT_Symbol;
}


bool  SYN::Syn::CanStartExp(TT tt)
{
  return (tt & TT_PREFIX) == TT_PREFIX;
}


bool  SYN::Syn::CanContainExp(TT tt)
{
  return (tt & TT_EXP) != 0;
}


bool  SYN::Syn::IsGlobalKeyword(TT tt)
{
  return (tt & TT_GLOBAL) == TT_GLOBAL;
}


void  SYN::Syn::nextToken()
{
  tt = lex->nextToken();
  lc = lex->ttBegLC();
}


bool  SYN::Syn::skipToken(TT tt, error_t err)
{
  if (this->tt == tt) {
    nextToken();
    return true;
  }
  else {
    error(err);
    return false;
  }
}


void  SYN::Syn::error(error_t err)
{
  errorHandler->error(lc, err);
}


bool  SYN::Syn::CanStartType(TT tt)
{
  return (tt == TT_Symbol);
}


void  SYN::Syn::skipToNextStm() //error recovery
{
  while (CanContainExp(tt))
    nextToken();
}


void  SYN::Syn::skipToNextClassDecl()
{
  while (tt != TT_class && tt != TT_interface && tt != TT_EOF) {
    nextToken();
  }
}


void  SYN::Syn::InitSymbolTable(SymbolTable* table)
{
#define KW(K) ((Symbol*)(table->getSymbol(#K, sizeof(#K)-1) - offsetof(Symbol, strid)))->keyword = TT_##K
  KW(package         );
  KW(import          );

  KW(class           );
  KW(interface       );

  KW(extends         );
  KW(implements      );

  KW(break           );
  KW(case            );
  KW(catch           );
  KW(continue        );
  KW(default         );
  KW(do              );
  KW(else            );
  KW(finally         );
  KW(for             );
  KW(if              );
  KW(return          );
  KW(switch          );
  KW(throw           );
  KW(throws          );
  KW(try             );
  KW(while           );

  KW(void            );
  KW(byte            );
  KW(short           );
  KW(int             );
  KW(long            );
  KW(float           );
  KW(double          );
  KW(boolean         );
  KW(char            );
  KW(true            );
  KW(false           );
  KW(null            );
  KW(new             );
//  KW(super           );// in non-static methods only!
//  KW(this            );// in non-static methods only!
  KW(instanceof      );

  KW(public       );
  KW(private      );
  KW(protected    );
  KW(static       );
  KW(final        );
  KW(synchronized );
  KW(threadsafe   );
  KW(transient    );
  KW(native       );
  KW(abstract     );
#undef KW
};

SYN::Syn::Syn()
  : lex(0),
    errorHandler(0),
    symbolTable(0),
    module(0)
{
}


SYN::Syn::Syn(Source* source, ErrorHandler* errorHandler)
  : errorHandler(errorHandler)
{
  symbolTable = new SymbolTable();
  InitSymbolTable(symbolTable);
  lex = new Lex();
  lex->source       = source;
  lex->errorHandler = errorHandler;
  lex->symbolTable  = symbolTable;
  nextToken();
  module = new Module();
}


SYN::Syn::~Syn()
{
  delete lex;
  delete symbolTable;
  delete module;
}

