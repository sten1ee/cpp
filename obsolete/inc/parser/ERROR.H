#ifndef PARSER_ERROR_H
#define PARSER_ERROR_H

enum error_t
{
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

  SErr_NewMissingClassId,
  SErr_ArglMissingOpenBrek,
  SErr_ArglMissingComma,
  SErr_ArglMissingClosBrek,
  SErr_MissingClosKvadBrek,
  SErr_MissingClosBrek,
  SErr_FuncIdExpected,
  SErr_IdentExpectedOnLeftOfDot,
  SErr_IdentExpectedOnRightOfDot,
  SErr_TypeExpectedOnLeft,
  SErr_ExpExpectedInsteadEOF,
  SErr_TermExpected,
  SErr_ArrNewInvalidDim,
  SErr_QMMissingDD,
  SErr_ArrAggMisingClosFigBrek,
  SErr_BadInitializer,

  Err_IfMissingOpenBrek,
  Err_IfMissingClosBrek,
  Err_WhileMissingOpenBrek,
  Err_WhileMissingClosBrek,
  Err_DoWhileMissingOpenBrek,
  Err_DoMissingWhile,
  Err_DoWhileMissingClosBrek,
  Err_SwitchMissingOpenBrek,
  Err_SwitchMissingClosBrek,
  Err_SynchroMissingOpenBrek,
  Err_SynchroMissingClosBrek,
  Err_CaseMissingDoubleDot,
  Err_DefaultMissingDoubleDot,
  Err_StatementMissingSemicol,
  Err_VarDeclExpected,
  Err_VarDeclMissingClosKvadBrek,
  Err_VarDeclMissingComma,
  Err_ForMissingOpenBrek,
  Err_ForMissingSemicol,
  Err_ForMissingClosBrek,
  Err_ThrowStmMissingSemicol,
  Err_CatchMissingOpenBrek,
  Err_MissingCatchParam,
  Err_TooManyCatchParams,
  Err_TryMissingHandlers,
  Err_CompoStmMissingClosFigBrek,
  Err_PossiblyMissingClosFigBrek,
  Err_StmExpected,
  Err_ParamTypeExpected,
  Err_ParamIdExpected,
  Err_ParamMissingClosKvadBrek,
  Err_ParamListMissingComma,
  Err_IdentExpected,
  Err_SemicolExpected,

  Err_SwitchMissingOpenFigBrek,
  Err_SynchroMissingOpenFigBrek,
  Err_TryMissingOpenFigBrek,
  Err_CatchMissingOpenFigBrek,
  Err_FinallyMissingOpenFigBrek,

  Err_ClassDeclExpected,
  Err_ClassDeclMissingClosFigBrek,
  Err_MissingComma,
  Err_BadMethodDecl,
  Err_MethodMissingReturnType,
  Err_MethodMissingBody,
  Err_CtorMissingBody,
  Err_MethodCantHaveBody,
  Err_DuplicateModifier
};

class ostream;
class LinCol;

class ErrorHandler
{
  private:
    ostream&  os;
  public:
    unsigned  nerrors;

    ErrorHandler(ostream& os) : os(os), nerrors(0) {}
    void      error(const LinCol& lc, error_t err);
    unsigned  nErrors() const { return nerrors; }
};

#endif //PARSER_ERROR_H
