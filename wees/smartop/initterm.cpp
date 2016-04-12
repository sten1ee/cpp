#include "initterm.h"

ProTerm
  ptIdent(""), ptNumber("");
/*
  ptOpenBrek("("), ptClosBrek(")"),
  ptOpenKvadBrek("["), ptClosKvadBrek("]"),
  ptDot("."), ptArrow("->"),

  ptLogNot("!"), ptBitNot("~"),
  ptInc("++"), ptDec("--"),
  ptBitAnd("&"), ptMul("*"),
  ptAdd("+"), ptSub("-"),

  //ptMul
  ptDiv("/"), ptMod("%"),

  //ptAdd, ptSub,

  ptShL("<<"), ptShR(">>"),

  ptLess("<"), ptLessEq("<="), ptGrtr(">"), ptGrtrEq(">="),

  ptEq("=="), ptNotEq("!="),

  //ptBitAnd

  ptBitXor("^"),

  ptBitOr("|"),

  ptLogAnd("&&"),

  ptLogOr("||"),

  ptAsg("="), ptMulAsg("*="), ptDivAsg("/="), ptModAsg("%="),
  ptAddAsg("+="), ptSubAsg("-="),
  ptShLAsg("<<="), ptShRAsg(">>="),
  ptAndAsg("&="), ptXorAsg("^="), ptOrAsg("|="),

  ptComma(","),

  ptQMark("?"), ptDblDot(":");
*/


void InitProTerms()
{
/*
  ptOpenBrek    .AddTerm(TT_Infix  , OI_FuncCall    ,  2, 99);
  ptOpenBrek    .AddTerm(TT_Prefix , OI_OpenBrek    ,  0, 99);
  ptClosBrek    .AddTerm(TT_Postfix, OI_ClosBrek    , 99,  0);

  ptOpenKvadBrek.AddTerm(TT_Infix  , OI_ArrIdx      ,  2, 99);
  ptClosKvadBrek.AddTerm(TT_Postfix, OI_ClosKvadBrek, 99,  0);

  ptDot         .AddTerm(TT_Infix  , OI_Dot         ,  2,  1);
  ptArrow       .AddTerm(TT_Infix  , OI_Arrow       ,  2,  1);

  ptInc         .AddTerm(TT_Postfix, OI_PosInc      ,  2,  0);
  ptDec         .AddTerm(TT_Postfix, OI_PosDec      ,  2,  0);

  ptInc         .AddTerm(TT_Prefix , OI_PreInc      ,  0,  3);
  ptDec         .AddTerm(TT_Prefix , OI_PreDec      ,  0,  3);
  ptLogNot      .AddTerm(TT_Prefix , OI_LogNot      ,  0,  3);
  ptBitNot      .AddTerm(TT_Prefix , OI_BitNot      ,  0,  3);
  ptBitAnd      .AddTerm(TT_Prefix , OI_Ref         ,  0,  3);
  ptMul         .AddTerm(TT_Prefix , OI_Deref       ,  0,  3);
  ptAdd         .AddTerm(TT_Prefix , OI_PrePlus     ,  0,  3);
  ptSub         .AddTerm(TT_Prefix , OI_PreMinus    ,  0,  3);

  ptMul         .AddTerm(TT_Infix  , OI_Mul         ,  5,  4);
  ptDiv         .AddTerm(TT_Infix  , OI_Div         ,  5,  4);
  ptMod         .AddTerm(TT_Infix  , OI_Mod         ,  5,  4);

  ptAdd         .AddTerm(TT_Infix  , OI_Add         ,  7,  6);
  ptSub         .AddTerm(TT_Infix  , OI_Sub         ,  7,  6);

  ptShL         .AddTerm(TT_Infix  , OI_ShL         ,  9,  8);
  ptShR         .AddTerm(TT_Infix  , OI_ShR         ,  9,  8);

  ptLess        .AddTerm(TT_Infix  , OI_Less        , 11, 10);
  ptLessEq      .AddTerm(TT_Infix  , OI_LessEq      , 11, 10);
  ptGrtr        .AddTerm(TT_Infix  , OI_Grtr        , 11, 10);
  ptGrtrEq      .AddTerm(TT_Infix  , OI_GrtrEq      , 11, 10);


  ptEq          .AddTerm(TT_Infix  , OI_Eq          , 13, 12);
  ptNotEq       .AddTerm(TT_Infix  , OI_NotEq       , 13, 12);

  ptBitAnd      .AddTerm(TT_Infix  , OI_BitAnd      , 15, 14);

  ptBitXor      .AddTerm(TT_Infix  , OI_BitXor      , 17, 16);

  ptBitOr       .AddTerm(TT_Infix  , OI_BitOr       , 19, 18);

  ptLogAnd      .AddTerm(TT_Infix  , OI_LogAnd      , 21, 20);

  ptLogOr       .AddTerm(TT_Infix  , OI_LogOr       , 23, 22);

  ptAsg         .AddTerm(TT_Infix  , OI_Asg         , 23, 24);
  ptMulAsg      .AddTerm(TT_Infix  , OI_MulAsg      , 23, 24);
  ptDivAsg      .AddTerm(TT_Infix  , OI_DivAsg      , 23, 24);
  ptModAsg      .AddTerm(TT_Infix  , OI_ModAsg      , 23, 24);
  ptAddAsg      .AddTerm(TT_Infix  , OI_AddAsg      , 23, 24);
  ptSubAsg      .AddTerm(TT_Infix  , OI_SubAsg      , 23, 24);
  ptShLAsg      .AddTerm(TT_Infix  , OI_ShLAsg      , 23, 24);
  ptShRAsg      .AddTerm(TT_Infix  , OI_ShRAsg      , 23, 24);
  ptAndAsg      .AddTerm(TT_Infix  , OI_AndAsg      , 23, 24);
  ptXorAsg      .AddTerm(TT_Infix  , OI_XorAsg      , 23, 24);
  ptOrAsg       .AddTerm(TT_Infix  , OI_OrAsg       , 23, 24);

  ptComma       .AddTerm(TT_Infix  , OI_Comma       , 25, 26);

  //ptQMark       .AddTerm(TT_Infix  , OI_QMark       , 29, 30);
  //ptDblDot      .AddTerm(TT_Infix  , OI_DblDot      , 30, 28);

  ptQMark       .AddTerm(TT_Infix  , OI_QMark       , 28, 30);
  ptDblDot      .AddTerm(TT_Infix  , OI_DblDot      , 30, 29);
*/
  ptIdent       .AddTerm(TT_Term   , OI_Ident       ,  0,  0);
  ptNumber      .AddTerm(TT_Term   , OI_Number      ,  0,  0);
}

