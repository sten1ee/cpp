//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parser/stmread.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include  "parser/syn.h"

AbstractSyntax::Stm*  SYN::Syn::ReadStm()
{
  switch (tt) {
    case TT_if: {
      IfStm* ifStm = new IfStm(lc);
      nextToken();
      skipToken(TT_OpenBrek, Err_IfMissingOpenBrek);
      ifStm->condExp = ReadExp();
      skipToken(TT_ClosBrek, Err_IfMissingClosBrek);
      ifStm->thenStm = ReadStm();
      if (tt == TT_else) {
        nextToken();
        ifStm->elseStm = ReadStm();
      }
      return ifStm;
    }

    case TT_while: {
      WhileStm* whileStm = new WhileStm(lc);
      nextToken();
      skipToken(TT_OpenBrek, Err_WhileMissingOpenBrek);
      whileStm->condExp = ReadExp();
      skipToken(TT_ClosBrek, Err_WhileMissingClosBrek);
      whileStm->bodyStm = ReadStm();
      return whileStm;
    }

    case TT_switch: {
      SwitchStm* switchStm = new SwitchStm(lc);
      nextToken();
      skipToken(TT_OpenBrek, Err_SwitchMissingOpenBrek);
      switchStm->switchedExp = ReadExp();
      skipToken(TT_ClosBrek, Err_SwitchMissingClosBrek);
      if (tt != TT_OpenFigBrek) {
        error(Err_SwitchMissingOpenFigBrek);
      }
      switchStm->bodyStm = ReadStm();
      return switchStm;
    }

    case TT_synchronized: {
      SynchroStm* synchroStm = new SynchroStm(lc);
      nextToken();
      skipToken(TT_OpenBrek, Err_SynchroMissingOpenBrek);
      synchroStm->synchroExp = ReadExp();
      skipToken(TT_ClosBrek, Err_SynchroMissingClosBrek);
      if (tt != TT_OpenFigBrek) {
        error(Err_SynchroMissingOpenFigBrek);
      }
      synchroStm->bodyStm = ReadStm();
      return synchroStm;
    }

    case TT_do: {
      DoWhileStm* doWhileStm = new DoWhileStm(lc);
      nextToken();
      doWhileStm->bodyStm = ReadStm();
      if (skipToken(TT_while, Err_DoMissingWhile)) {
        skipToken(TT_OpenBrek, Err_DoWhileMissingOpenBrek);
      }
      doWhileStm->condExp = ReadExp();
      skipToken(TT_ClosBrek, Err_DoWhileMissingClosBrek);
      skipToken(TT_Semicol,  Err_StatementMissingSemicol);
      return doWhileStm;
    }

    case TT_for: {
      ForStm* forStm = new ForStm(lc);
      nextToken();
      skipToken(TT_OpenBrek, Err_ForMissingOpenBrek);
      if (tt == TT_Semicol)
        nextToken();
      else
        forStm->initStm = ReadDeclOrExpStm(false); //can be labeled: false

      if (tt != TT_Semicol) {
        forStm->condExp = ReadExp();
      }
      skipToken(TT_Semicol,  Err_ForMissingSemicol);

      if (tt != TT_ClosBrek) {
        Exp* alterExp = ReadExp();
        if (tt != TT_Comma) {
          forStm->alterStm = new ExpStm(alterExp->lc, alterExp);
        }
        else {
          CompoStm* compoStm = new CompoStm(alterExp->lc);
          forStm->alterStm = compoStm;
          compoStm->stm.push(new ExpStm(alterExp->lc, alterExp));
          do {
            nextToken();
            alterExp = ReadExp();
            compoStm->stm.push(new ExpStm(alterExp->lc, alterExp));
          } while (tt == TT_Comma);
        }
      }
      skipToken(TT_ClosBrek,  Err_ForMissingClosBrek);

      forStm->bodyStm = ReadStm();
      return forStm;
    }

    case TT_throw: {
      ThrowStm* throwStm = new ThrowStm(lc);
      nextToken();
      throwStm->thrownExp = ReadExp();
      skipToken(TT_Semicol, Err_ThrowStmMissingSemicol);
      return throwStm;
    }

    case TT_try: {
      TryStm* tryStm = new TryStm(lc);
      nextToken();
      if (tt != TT_OpenFigBrek) {
        error(Err_TryMissingOpenFigBrek);
      }
      tryStm->triedStm = ReadStm();
      while (tt == TT_catch) {
        CatchClause* catchClause = new CatchClause(lc);
        tryStm->catchClause.push(catchClause);
        nextToken();
        if (tt != TT_OpenBrek) {
          error(Err_CatchMissingOpenBrek);
        }
        else {
          VarDecl::Vector  param = ReadParamList();
          if (param.size() == 0) {
            error(Err_MissingCatchParam);
          }
          else if (param.size() > 1) {
            error(Err_TooManyCatchParams);
          }
          else {
            catchClause->param = param.pop();
          }
          if (tt != TT_OpenFigBrek) {
            error(Err_CatchMissingOpenFigBrek);
          }
          catchClause->stm = ReadStm();
        }
      }
      if (tt == TT_finally) {
        nextToken();
        if (tt != TT_OpenFigBrek) {
          error(Err_FinallyMissingOpenFigBrek);
        }
        tryStm->finallyStm = ReadStm();
      }
      else if (tryStm->catchClause.isEmpty()) {
        error(Err_TryMissingHandlers);
      }
      return tryStm;
    }

    case TT_break: {
      BreakStm* breakStm = new BreakStm(lc);
      nextToken();
      if (tt == TT_Symbol) {
        breakStm->labelSym = lex->valueSymbol();
        nextToken();
      }
      skipToken(TT_Semicol, Err_StatementMissingSemicol);
      return breakStm;
    }

    case TT_continue: {
      ContinueStm* continueStm = new ContinueStm(lc);
      nextToken();
      if (tt == TT_Symbol) {
        continueStm->labelSym = lex->valueSymbol();
        nextToken();
      }
      skipToken(TT_Semicol, Err_StatementMissingSemicol);
      return continueStm;
    }

    case TT_return: {
      ReturnStm* returnStm = new ReturnStm(lc);
      nextToken();
      if (tt != TT_Semicol) {
        returnStm->returnedExp = ReadExp();
      }
      skipToken(TT_Semicol, Err_StatementMissingSemicol);
      return returnStm;
    }

    case TT_case: {
      CaseStm* caseStm = new CaseStm(lc);
      nextToken();
      caseStm->casedExp = ReadExp();
      skipToken(TT_DoubleDot, Err_CaseMissingDoubleDot);
      return caseStm;
    }

    case TT_default: {
      DefaultStm* defaultStm = new DefaultStm(lc);
      nextToken();
      skipToken(TT_DoubleDot, Err_DefaultMissingDoubleDot);
      return defaultStm;
    }

    case TT_OpenFigBrek: {
      CompoStm* compoStm = new CompoStm(lc);
      nextToken();
      Stm* stm = 0;
      while (true) {
        while (tt == TT_Semicol) {
          nextToken();
        }
        if (tt == TT_ClosFigBrek) {
          nextToken();
          break;
        }
        if (IsGlobalKeyword(tt)) { //prevent infinite loop
          error(Err_CompoStmMissingClosFigBrek);
          break;
        }
        compoStm->stm.push(stm = ReadStm());
      }
      if (stm == 0) {
        stm = new EmptyStm(compoStm->lc);
        delete compoStm;
      }
      else if (compoStm->stm.size() == 1) {
        stm = compoStm->stm.pop();
        delete compoStm;
      }
      else {
        stm = compoStm;
      }
      return stm;
    }

    case TT_Semicol: {
      Stm* stm = new EmptyStm(lc);
      nextToken();
      return stm;
    }

    default: {
      if (CanStartLocalDecl(tt) || CanStartExp(tt)) {
        return ReadDeclOrExpStm(true); //can be labeled: true
      }
      else {
        if (IsGlobalKeyword(tt)) {
          error(Err_PossiblyMissingClosFigBrek);
        }
        else {
          error(Err_StmExpected);
          nextToken();
        }
        return new EmptyStm(lc);
      }
    }
  }
}


AbstractSyntax::Stm*  SYN::Syn::ReadDeclOrExpStm(bool bAllowLablels)
{
  LinCol    _lc;
  Exp*      exp    = 0;
  int       modifs = ReadModifs();
  TypeExp*  typeExp= 0;
  if (modifs) {
    typeExp = ReadTypeExp();
  }
  else {
    exp = ReadExp();
    IdExp* idExp = SAFE_CAST(exp, IdExp);
    if (idExp) {
      SymExp* symExp = SAFE_CAST(idExp, SymExp);
      if (symExp) {
        if (bAllowLablels && tt == TT_DoubleDot) { //label:
          nextToken();
          LabelStm* labelStm = new LabelStm(symExp->lc, symExp->sym);
          delete symExp;
          labelStm->labeledStm = ReadStm();
          return labelStm;
        }
      }
      exp = new ClassTypeExp(_lc, idExp);
    }
    typeExp = SAFE_CAST(exp, TypeExp);
  }
  if (typeExp) {
    DeclStm* declStm = new DeclStm(_lc, modifs, typeExp);
    while (true) {
      if (tt != TT_Symbol) {
        error(Err_VarDeclExpected);
        skipToNextStm();
        break;
      }
      VarDecl* varDecl = ReadVarDecl(typeExp, modifs);
      declStm->varDecl.push(varDecl);
      if (tt == TT_Semicol) {
        nextToken();
        break;
      }
      if (!skipToken(TT_Comma, Err_VarDeclMissingComma)) {
        skipToNextStm();
        break;
      }
    }
    return declStm;
  }
  else {
    if (tt == TT_Semicol)
      nextToken();
    else
      error(Err_StatementMissingSemicol);
    return new ExpStm(_lc, exp);
  }
}


AbstractSyntax::VarDecl* SYN::Syn::ReadVarDecl(TypeExp* typeExp, int modifs)
{
  PARSER_ASSERT(tt == TT_Symbol);
  VarDecl* varDecl = new VarDecl(lc, DT_LOCALVAR);
  varDecl->sym = lex->valueSymbol();
  nextToken();
  varDecl->modifs  = modifs;
  varDecl->typeExp = typeExp;
  typeExp->nref++;
  varDecl->xdims = ReadDims();
  if (tt == TT_Assign) {
    nextToken();
    if (tt != TT_OpenFigBrek) {
      varDecl->initExp = ReadExp();
    }
    else {
      int ndims = varDecl->xdims;
      if (SAFE_CAST(typeExp, ArrTypeExp)) {
        ndims += SURE_CAST(typeExp, ArrTypeExp)->ndims;
      }
      if (ndims == 0) {
        error(SErr_BadInitializer);
      }
      else {
        varDecl->initExp = ReadArrAggExp(ndims);
      }
    }
  }
  return varDecl;
}


AbstractSyntax::VarDecl::Vector  SYN::Syn::ReadParamList()
{
  PARSER_ASSERT(tt == TT_OpenBrek);
  nextToken();

  VarDecl::Vector  res;

  if (tt == TT_ClosBrek) {
    nextToken();
    return res;
  }
  while (true) {
    LinCol  _lc;
    Exp* exp = ReadExp();
    TypeExp* typeExp = SAFE_CAST(exp, TypeExp);
    if (!typeExp) {
      IdExp* idExp = SAFE_CAST(exp, IdExp);
      if (idExp) {
        typeExp = new ClassTypeExp(_lc, idExp);
      }
      else {
        error(Err_ParamTypeExpected);
        delete exp;
        typeExp = new StubTypeExp(_lc);
      }
    }

    VarDecl* param = new VarDecl(lc, DT_PARAMVAR);
    res.push(param);
    param->typeExp = typeExp;
    typeExp->nref++;

    if (tt == TT_Symbol) {
      param->sym = lex->valueSymbol();
      nextToken();
    }
    else {
      error(Err_ParamIdExpected);
      param->sym = TheStubSymbol;
    }

    param->xdims = ReadDims();

    if (tt == TT_ClosBrek) {
      nextToken();
      break;
    }
    else if (!skipToken(TT_Comma, Err_ParamListMissingComma)) {
      skipToNextStm();
      break;
    }
  }

  return res;
}
