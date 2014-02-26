#include "parser/syn.h"


void  SYN::Syn::ReadModule()
{
  if (tt == TT_package) {
    nextToken();
    //read package name
    module->packageId = ReadIdExp(Err_IdentExpected);
    skipToken(TT_Semicol, Err_SemicolExpected);
  }
  while (tt == TT_import) {
    IdExp*  idExp = 0;
    bool    bPackage = false;
    while (true) {
      nextToken();
      if (tt == TT_Symbol) {
        if (idExp) {
          idExp = new QSymExp(lc, idExp, lex->valueSymbol());
        }
        else {
          idExp = new SymExp(lc, lex->valueSymbol());
        }
        nextToken();
        if (tt != TT_Dot) {
          if (!idExp) {
            error(Err_IdentExpected);
          }
          skipToken(TT_Semicol, Err_SemicolExpected);
          break;
        }
      }
      else if (tt == TT_Mul) {
        if (!idExp) {
          error(Err_IdentExpected);
        }
        bPackage = true;
        nextToken();
        skipToken(TT_Semicol, Err_SemicolExpected);
        break;
      }
      else {
        if (idExp) {
          delete idExp;
          idExp = 0;
        }
        skipToken(TT_Semicol, Err_SemicolExpected);
        error(Err_IdentExpected);
        break;
      }
    }
    //read import
    if (idExp) {
      module->addImport(idExp, bPackage);
    }
    else {
      break;
    }
  }
  while (tt != TT_EOF) {
    if (tt == TT_Semicol) {
      nextToken();
    }
    else if (false == ReadClassDecl()) {
      skipToNextClassDecl();
    }
  }
}


bool  SYN::Syn::ReadClassDecl()
{
  int modifs = ReadModifs();
  switch (tt) {
    case TT_interface:  modifs |= M_INTERFACE; //no break !
    case TT_class:      nextToken();
                        break;
    default :           error(Err_ClassDeclExpected);
                        return false;
  }
  if (tt != TT_Symbol) {
    error(Err_IdentExpected);
    return false;
  }
  symbol_t  classSym = lex->valueSymbol();
  ClassDecl* classDecl = new ClassDecl(lc, modifs, classSym);
  module->addClassDecl(classDecl);
  nextToken();
  if (tt == TT_extends) {
    nextToken();
    classDecl->baseClassId = ReadIdExp(Err_IdentExpected);
  }
  if (tt == TT_implements) {
    nextToken();
    while (true) {
      classDecl->addInterface(ReadIdExp(Err_IdentExpected));
      if (tt == TT_OpenFigBrek) {
        break;
      }
      if (tt != TT_Comma) {
        error(Err_MissingComma);
        return false;
      }
      nextToken();
    }
  }
  if (tt != TT_OpenFigBrek) {
    return false;
  }
  nextToken();
  while (true) {
    if (tt == TT_ClosFigBrek) {
      nextToken();
      return true;
    }
    if (tt == TT_class || tt == TT_interface || tt == TT_EOF) {
      error(Err_ClassDeclMissingClosFigBrek);
      return true;
    }
    if (false == ReadMemberDecl()) {
      return false; //skipToNextMemberDecl();
    }
  }
}


bool  SYN::Syn::ReadMemberDecl()
{
  while (tt == TT_Semicol) {
    nextToken();
  }
  if (tt == TT_ClosFigBrek) {
    return false;
  }
  int       modifs  =ReadModifs();
  LinCol    _lc     =lc;
  if (tt == TT_OpenFigBrek) {
    classDecl()->addStaticBlock(new StaticBlock(lc, modifs, ReadStm()));
    return true;
  }
  TypeExp*  typeExp =ReadTypeExp();
  if (tt == TT_OpenBrek) { //constructor
    ClassTypeExp* classTypeExp = SAFE_CAST(typeExp, ClassTypeExp);
    symbol_t  sym = 0;
    if (!classTypeExp) {
      error(Err_BadMethodDecl);
    }
    else {
      SymExp* symExp = SAFE_CAST(classTypeExp->classIdExp, SymExp);
      if (!symExp) {
        error(Err_BadMethodDecl);
      }
      else {
        sym = symExp->sym;
      }
    }
    delete typeExp;
    if (!sym) {
      return false;
    }
    if (sym != classDecl()->sym) {
      error(Err_MethodMissingReturnType);
      return false;
    }
    MethodDecl* ctor = new MethodDecl(_lc, DT_CTOR, modifs, sym);
    ctor->param = ReadParamList();
    classDecl()->addMethod(ctor);
    if (tt == TT_throws) {
      ctor->throwsClass = ReadThrowsClass();
    }
    if (tt != TT_OpenFigBrek) {
      error(Err_CtorMissingBody);
      return false;
    }
    ctor->bodyStm  = ReadStm();
    return true;
  }
  if (tt != TT_Symbol) {
    error(Err_IdentExpected);
    delete typeExp;
    return false;
  }
  symbol_t  membsym = lex->valueSymbol();
  nextToken();
  if (tt == TT_OpenBrek) { //read method
    MethodDecl* method = new MethodDecl(lc, DT_METHOD, modifs, membsym);
    method->typeExp = typeExp;
    method->param = ReadParamList();
    classDecl()->addMethod(method);
    if (tt == TT_OpenKvadBrek) {
      method->xdims = ReadDims();
    }
    if (tt == TT_throws) {
      method->throwsClass = ReadThrowsClass();
    }
    if (tt == TT_OpenFigBrek) {
      method->bodyStm = ReadStm();
    }
    else {
      skipToken(TT_Semicol, Err_StatementMissingSemicol);
    }
    return true;
  }
  //loop to read member vars:
  while (true) {
    VarDecl* var = new VarDecl(lc, DT_INSTVAR);
    classDecl()->addVar(var);
    var->modifs = modifs;
    var->typeExp = typeExp;
    typeExp->nref++;
    var->sym = membsym;
    var->xdims = ReadDims();
    if (tt == TT_Assign) {
      nextToken();
      if (tt != TT_OpenFigBrek) {
        var->initExp = ReadExp();
      }
      else {
        int ndims = var->xdims;
        if (SAFE_CAST(typeExp, ArrTypeExp)) {
          ndims = SURE_CAST(typeExp, ArrTypeExp)->ndims;
        }
        if (ndims == 0) {
          error(SErr_BadInitializer);
        }
        else {
          var->initExp = ReadArrAggExp(ndims);
        }
      }
    }
    if (tt == TT_Semicol) {
      nextToken();
      return true;
    }
    if (tt != TT_Comma) {
      error(Err_MissingComma);
      return false;
    }
    nextToken();
    if (tt != TT_Symbol) {
      error(Err_IdentExpected);
      return false;
    }
    membsym = lex->valueSymbol();
    nextToken();
  }
}


AbstractSyntax::ClassTypeExp::Vector SYN::Syn::ReadThrowsClass()
{
  PARSER_ASSERT(tt == TT_throws);
  nextToken();
  ClassTypeExp::Vector  res;
  while (true) {
    res.push(ReadClassTypeExp());
    if (tt == TT_Comma) {
      nextToken();
    }
    else {
      break;
    }
  }
  return res;
}

