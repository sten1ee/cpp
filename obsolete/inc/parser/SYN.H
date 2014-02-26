#ifndef PARSER_SYN_H
#define PARSER_SYN_H

#include "parser/lex.h"
#include "parser/error.h"
#include "parser/structs.h"

//namespace emulation:
class  SYN : public AbstractSyntax  {  public:
//---------------------------------------------------------

class Syn {
  public:
    TT            tt;
    LinCol        lc;
    Lex*          lex;
    ErrorHandler* errorHandler;
    SymbolTable*  symbolTable;
    Module*       module;

                  Syn();
                  Syn(Source* source, ErrorHandler* errorHandler);
                 ~Syn();
    void          ReadModule();

   protected:
    void          nextToken();
    ClassDecl*    classDecl();

    void    error(error_t err);
    void    skipToNextStm();
    void    skipToNextMemberDecl();
    void    skipToNextClassDecl();
    bool    skipToken(TT tt, error_t err);

    static  bool    CanStartType(TT);
    static  bool    CanStartLocalDecl(TT);
    static  bool    CanStartExp(TT);
    static  bool    CanContainExp(TT);
    static  bool    IsGlobalKeyword(TT);
    static  bool    IsPrimeType(TT);
    static  PT      TT2PT(TT);
    static  void    InitSymbolTable(SymbolTable* table);

    Exp* ReadLevel_Assigns();
    Exp* ReadLevel_LogOrs();
    Exp* ReadLevel_LogAnds();
    Exp* ReadLevel_InstanceOf();
    Exp* ReadLevel_BitOrs();
    Exp* ReadLevel_BitXors();
    Exp* ReadLevel_BitAnds();
    Exp* ReadLevel_Eqs();
    Exp* ReadLevel_Comps();
    Exp* ReadLevel_Shifts();
    Exp* ReadLevel_Adds();
    Exp* ReadLevel_Muls();
    Exp* ReadLevel_Prefixes();
    Exp* ReadLevel_Postfixes();
    Exp* ReadLevel_Scopes();
    Exp* ReadLevel_Term();

    VarDecl*        ReadVarDecl(TypeExp* varTypeExp, int modifs);
    VarDecl::Vector ReadParamList();
    Exp::Vector     ReadArgList();
    ClassTypeExp::Vector  ReadThrowsClass();

    Exp*            ReadArrAggExp(int ndims);
    Stm*            ReadDeclOrExpStm(bool bAllowLabels);
    Exp*            ReadNewExp      ();
    IdExp*          ReadIdExp       (error_t err=ErrNone);
    ClassTypeExp*   ReadClassTypeExp(error_t err=ErrNone);
    ObjTypeExp*     ReadObjTypeExp  (error_t err=ErrNone);
    TypeExp*        ReadTypeExp     (error_t err=ErrNone);
    int             ReadDims        ();
    int             ReadModifs      ();

    bool            ReadClassDecl();
    bool            ReadMemberDecl();
  public:
    Exp* ReadExp();
    Stm* ReadStm();
};

}; //end namespace SYN
//---------------------------------------------------------

inline
AbstractSyntax::ClassDecl* SYN::Syn::classDecl()
{
  PARSER_ASSERT(!module->classDecl.isEmpty());
  return module->classDecl.top();
}

#endif //PARSER_SYN_H
