#if !defined(__INITTERM_H)
#    define  __INITTERM_H

#include "smartop.h"
#include "parsetre.h"
#include <string.h>

void InitProTerms(void);
Term* getTerm(TopParserNode* topNode);

enum _OPID {
  OI_NOP         ,
  OI_FuncCall    ,//  2, 99);
  OI_OpenBrek    ,// 99, 99);
  OI_ClosBrek    ,// 99, 99);

  OI_ArrIdx      ,//  2, 99);
  OI_ClosKvadBrek,// 99, 99);

  OI_Dot         ,//  2,  1);
  OI_Arrow       ,//  2,  1);

  OI_PosInc      ,//  2,  0);
  OI_PosDec      ,//  2,  0);

  OI_PreInc      ,//  0,  3);
  OI_PreDec      ,//  0,  3);
  OI_LogNot      ,//  0,  3);
  OI_BitNot      ,//  0,  3);
  OI_Ref         ,//  0,  3);
  OI_Deref       ,//  0,  3);
  OI_PrePlus     ,//  0,  3);
  OI_PreMinus    ,//  0,  3);

  OI_Mul         ,//  5,  4);
  OI_Div         ,//  5,  4);
  OI_Mod         ,//  5,  4);

  OI_Add         ,//  7,  6);
  OI_Sub         ,//  7,  6);

  OI_ShL         ,//  9,  8);
  OI_ShR         ,//  9,  8);

  OI_Less        ,// 11, 10);
  OI_LessEq      ,// 11, 10);
  OI_Grtr        ,// 11, 10);
  OI_GrtrEq      ,// 11, 10);


  OI_Eq          ,// 13, 12);
  OI_NotEq       ,// 13, 12);

  OI_BitAnd      ,// 15, 14);

  OI_BitXor      ,// 17, 16);

  OI_BitOr       ,// 19, 18);

  OI_LogAnd      ,// 21, 20);

  OI_LogOr       ,// 23, 22);

  OI_Asg         ,// 23, 24);
  OI_MulAsg      ,// 23, 24);
  OI_DivAsg      ,// 23, 24);
  OI_ModAsg      ,// 23, 24);
  OI_AddAsg      ,// 23, 24);
  OI_SubAsg      ,// 23, 24);
  OI_ShLAsg      ,// 23, 24);
  OI_ShRAsg      ,// 23, 24);
  OI_AndAsg      ,// 23, 24);
  OI_XorAsg      ,// 23, 24);
  OI_OrAsg       ,// 23, 24);

  OI_Comma       ,// 25, 26);

  OI_QMark       ,// 99, 99);
  OI_DblDot      ,// 99, 99);

  OI_Ident       ,//  0,  0);
  OI_Number      ,//  0,  0);
};


extern ProTerm
  ptIdent, ptNumber,
  ptOpenBrek, ptClosBrek,
  ptOpenKvadBrek, ptClosKvadBrek,
  ptOpenFigBrek, ptClosFigBrek,
  ptDot, ptArrow,

  ptLogNot, ptBitNot,
  ptInc, ptDec,
  ptBitAnd, ptMul,
  ptAdd, ptSub,

  //ptMul
  ptDiv, ptMod,

  //ptAdd, ptSub,

  ptShL, ptShR,

  ptLess, ptLessEq, ptGrtr, ptGrtrEq,

  ptEq, ptNotEq,

  //ptBitAnd

  ptBitXor,

  ptBitOr,

  ptLogAnd,

  ptLogOr,

  ptAsg, ptMulAsg, ptDivAsg, ptModAsg, ptAddAsg, ptSubAsg,
  ptShLAsg, ptShRAsg, ptAndAsg, ptXorAsg, ptOrAsg,

  ptComma,

  ptQMark, ptDblDot;


class UniqTerm : public Term {
  public:
    char* name;
    UniqTerm(const char* name, ProTerm& pt)
      : Term(pt), name(strdup(name))
    {}
   ~UniqTerm() { free(name); }
};

class IdentTerm : public UniqTerm {
  public:

    IdentTerm(const char* name)
      : UniqTerm(name, ptIdent)
    {}
};

class NumberTerm : public UniqTerm {
  public:

    NumberTerm(const char* name)
      : UniqTerm(name, ptIdent)
    {}
};

#endif