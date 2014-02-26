#ifndef PARSER_STRUCTS_H
#define PARSER_STRUCTS_H

#include "util/vector.h"
#include "util/vectort.h"
#include "util/wstring.h"
#include "parser/lincol.h"
#include "parser/symtable.h"
#include "parser/long.h"

//namespace emulation:
class  AbstractSyntax  {  public:
//---------------------------------------------------------
///////////////////////////////////////////////////////////////////////
// @EXP
///////////////////////////////////////////////////////////////////////

enum ET {
  ET_NONE,
  ET_CALL,
  ET_CAST,
  ET_MEMBER,
  ET_CLASSNEW,
  ET_ARRNEW,
  ET_SPECNEW,
  ET_INSTOF,
  ET_SPECSYM,
  ET_IF,
  ET_ARRAGG,

  ET_BINEXP    = 0x0100,
  ET_UNEXP     = 0x0200,
  ET_IDEXP     = 0x0400,
  ET_TYPEEXP   = 0x0800,
  ET_LITERAL   = 0x1000,

  ET_PRIMETYPE = ET_TYPEEXP + 1,
  ET_CLASSTYPE,
  ET_ARRTYPE,
  ET_STUBTYPE,

  ET_SYM       = ET_IDEXP + 1,
  ET_QSYM,
  ET_STUBID,
  ET_MEMBSYM,

  ET_INCE      = ET_UNEXP + 1,
  ET_DECE,

  ET_EINC,
  ET_EDEC,
  ET_NOT,
  ET_BNOT,
  ET_UPLS,
  ET_UMNS,

  ET_SUBSCRIBE  = ET_BINEXP + 1,
  ET_MUL,
  ET_DIV,
  ET_MOD,

  ET_ADD,
  ET_SUB,

  ET_SHL,
  ET_SHR,
  ET_SHRL,

  ET_BAND,
  ET_BXOR,
  ET_BOR,

  ET_LESS,
  ET_LESSEQ,
  ET_GRTR,
  ET_GRTREQ,

  ET_EQ,
  ET_NEQ,

  ET_AND,
  ET_OR,

  ET_ASG,
  ET_MULASG,
  ET_DIVASG,
  ET_MODASG,
  ET_ADDASG,
  ET_SUBASG,
  ET_BANDASG,
  ET_BXORASG,
  ET_BORASG,
  ET_SHLASG,
  ET_SHRASG,
  ET_SHRLASG,

  ET_NULL      = ET_LITERAL + 1,
  ET_INT,
  ET_LONG,
  ET_FLOAT,
  ET_DOUBLE,
  ET_STRING,
  ET_CHAR
};


enum PT {
  PT_VOID,
  PT_BYTE,
  PT_SHORT,
  PT_INT,
  PT_LONG,
  PT_FLOAT,
  PT_DOUBLE,
  PT_CHAR,
  PT_BOOLEAN
};


enum SS {
  SS_NULL,
  SS_THIS,
  SS_SUPER,
  SS_TRUE,
  SS_FALSE
};


enum {
  M_PUBLIC       = 0x0001,
  M_PRIVATE      = 0x0002,
  M_PROTECTED    = 0x0004,
  M_STATIC       = 0x0008,
  M_FINAL        = 0x0010,
  M_SYNCHRONIZED = 0x0020,
  M_THREADSAFE   = 0x0040,
  M_TRANSIENT    = 0x0080,
  M_NATIVE       = 0x0100,
  M_INTERFACE    = 0x0200,
  M_ABSTRACT     = 0x0400
};


class Exp
{
  public:
    LinCol  lc;
    ET      et;

    virtual~Exp(){}
            Exp(LinCol lc, ET et)
              : lc(lc), et(et)
            {}

    typedef Vector< VectorPtrTraits<Exp> > Vector;
};


class DotExp : public Exp
{
  public:
    symbol_t  sym; //the symbol right to the dot

    DotExp(LinCol lc, ET et, symbol_t sym)
      : Exp(lc, et),
        sym(sym)
    {}
};


class IdExp : public DotExp
{
  protected:
    IdExp(LinCol lc, ET et, symbol_t sym)
      : DotExp(lc, et, sym)
    {}
};


class SymExp : public IdExp
{
  public:
    SymExp(LinCol lc, symbol_t sym)
      : IdExp(lc, ET_SYM, sym)
    {}
};


class QSymExp : public IdExp
{
  public:
    IdExp*  scopeIdExp;

   ~QSymExp();
    QSymExp(LinCol lc, IdExp* scopeIdExp, symbol_t sym)
      : IdExp(lc, ET_QSYM, sym),
        scopeIdExp(scopeIdExp)
      {}
};


class StubIdExp : public IdExp
{
  public:
    StubIdExp(LinCol lc)
      : IdExp(lc, ET_STUBID, TheStubSymbol)
    {}
};


class MembSymExp : public DotExp
{
  public:
    Exp*     objExp; //the expression left to the dot (surely an object)

   ~MembSymExp();
    MembSymExp(LinCol lc, Exp* objExp, symbol_t sym)
      : DotExp(lc, ET_MEMBSYM, sym),
        objExp(objExp)
      {}
};


class SpecSymExp : public Exp
{
  public:
    SS  ss;

    SpecSymExp(LinCol lc, SS ss)
      : Exp(lc, ET_SPECSYM),
        ss(ss)
      {}
};


class TypeExp : public Exp
{
  public:
    size_t  nref;

   ~TypeExp() { PARSER_ASSERT(nref == 0); }
    TypeExp(LinCol lc, ET et)
      : Exp(lc, et),
        nref(0)
    {}

    static  void  Free(TypeExp* typeExp)
      {
        if (--typeExp->nref == 0) delete typeExp;
      }
};


class PrimeTypeExp : public TypeExp
{
  public:
    PT  pt;

    static  const char*  PT2Str(PT tt);
            const char*  toStr() const { return PT2Str(pt); }  

    PrimeTypeExp(LinCol lc, PT pt)
      : TypeExp(lc, ET_PRIMETYPE),
        pt(pt)
    {}
};


class ObjTypeExp : public TypeExp
{
  public:
    ObjTypeExp(LinCol lc, ET et)
      : TypeExp(lc, et)
      {}
};


class ClassTypeExp : public ObjTypeExp
{
  public:
    IdExp* classIdExp;

   ~ClassTypeExp();
    ClassTypeExp(LinCol lc, IdExp* classIdExp)
      : ObjTypeExp(lc, ET_CLASSTYPE),
        classIdExp(classIdExp)
      {}

//    typedef Vector< VectorPtrTraits< ClassTypeExp > > Vector;
};


class ArrTypeExp : public ObjTypeExp
{
  public:
    TypeExp*  typeExp;
    int       ndims;

   ~ArrTypeExp();
    ArrTypeExp(LinCol lc, TypeExp* typeExp, int ndims)
      : ObjTypeExp(lc, ET_ARRTYPE),
        typeExp(typeExp), ndims(ndims)
      {}
};


class StubTypeExp : public TypeExp
{
  public:
    StubTypeExp(LinCol lc)
      : TypeExp(lc, ET_STUBTYPE)
      {}
};


class CastExp : public Exp
{
  public:
    TypeExp*  targetTypeExp;
    Exp*      castedExp;

   ~CastExp();
    CastExp(LinCol lc, TypeExp* targetTypeExp, Exp* castedExp)
      : Exp(lc, ET_CAST),
        targetTypeExp(targetTypeExp), castedExp(castedExp)
      {}
};


class CallExp : public Exp
{
  public:
    DotExp*       funcDotExp;
    Exp::Vector   argl;

   ~CallExp();
    CallExp(LinCol lc, DotExp* funcDotExp)
      : Exp(lc, ET_CALL),
        funcDotExp(funcDotExp)
      {}
};

class BinExp : public Exp
{
  public:
    Exp* lop;
    Exp* rop;

   ~BinExp();
    BinExp(LinCol lc, ET et, Exp* lop=0, Exp* rop=0)
      : Exp(lc, et), lop(lop), rop(rop)
      {}
};

class UnExp : public Exp
{
  public:
    Exp* op;

   ~UnExp();
    UnExp(LinCol lc, ET et, Exp* op=0)
      : Exp(lc, et), op(op)
      {}
};

class LiteralExp : public Exp
{
  public:
    LiteralExp(LinCol lc, ET et)
      : Exp(lc, et)
      {}
};


class IntExp : public LiteralExp
{
  public:
    uint32  val;

    IntExp(LinCol lc, uint32 val)
      : LiteralExp(lc, ET_INT),
        val(val)
      {}
};


class LongExp : public LiteralExp
{
  public:
    Long  val;

    LongExp(LinCol lc, Long val)
      : LiteralExp(lc, ET_LONG),
        val(val)
      {}
};


class FloatExp : public LiteralExp
{
  public:
    double  val;

    FloatExp(LinCol lc, float val)
      : LiteralExp(lc, ET_FLOAT),
        val(val)
      {}
};


class DoubleExp : public LiteralExp
{
  public:
    double  val;

    DoubleExp(LinCol lc, double val)
      : LiteralExp(lc, ET_DOUBLE),
        val(val)
      {}
};

class StringExp : public LiteralExp
{
  public:
    wstring  val;

    StringExp(LinCol lc, const wstring& val)
      : LiteralExp(lc, ET_STRING),
        val(val)
      {}
};

class CharExp : public LiteralExp
{
  public:
    wchar_t  val;

    CharExp(LinCol lc, wchar_t  val)
      : LiteralExp(lc, ET_CHAR),
        val(val)
      {}
};


class NewExp : public Exp
{
  public:
    NewExp(LinCol lc, ET et)
      : Exp(lc, et)
    {}
};


class ClassNewExp : public NewExp
{
  public:
    ClassTypeExp*   classTypeExp;
    Exp::Vector     argl;  //arguments for constructor

   ~ClassNewExp();
    ClassNewExp(LinCol lc, ClassTypeExp* classTypeExp, Exp::Vector& argl)
      : NewExp(lc, ET_CLASSNEW),
        classTypeExp(classTypeExp), argl(argl)
      {}
};


class ArrNewExp : public NewExp
{
  public:
    TypeExp*        typeExp;
    Exp::Vector     dim;

   ~ArrNewExp();
    ArrNewExp(LinCol lc, TypeExp* typeExp)
      : NewExp(lc, ET_ARRNEW),
        typeExp(typeExp), dim(0)
      {}
};


class SpecNewExp : public NewExp
{
  public:
    Exp* exp;

   ~SpecNewExp();
    SpecNewExp(LinCol lc, Exp* exp)
      : NewExp(lc, ET_SPECNEW),
        exp(exp)
      {}
};


class InstofExp : public Exp
{
  public:
    Exp*        checkedExp;
    ObjTypeExp* objTypeExp;

   ~InstofExp();
    InstofExp(LinCol lc, Exp* checkedExp)
      : Exp(lc, ET_INSTOF),
        checkedExp(checkedExp), objTypeExp(0)
      {}
};


class IfExp : public Exp
{
  public:
    Exp*  condExp;
    Exp*  thenExp;
    Exp*  elseExp;

   ~IfExp();
    IfExp(LinCol lc, Exp* condExp=0, Exp* thenExp=0, Exp* elseExp=0)
      : Exp(lc, ET_IF),
        condExp(condExp), thenExp(thenExp), elseExp(elseExp)
      {}
};


class ArrAggExp : public Exp
{
  public:
    Exp::Vector val;

    ArrAggExp(LinCol lc)
      : Exp(lc, ET_ARRAGG)
      {}
};

///////////////////////////////////////////////////////////////////////
// @STM
///////////////////////////////////////////////////////////////////////

enum  ST {
  ST_EMPTY,
  ST_IF,
  ST_WHILE,
  ST_DO_WHILE,
  ST_FOR,
  ST_TRY,
  ST_THROW,
  ST_DECL,
  ST_EXP,
  ST_COMPO,
  ST_SWITCH,
  ST_CASE,
  ST_DEFAULT,
  ST_LABEL,
  ST_BREAK,
  ST_CONTINUE,
  ST_RETURN,
  ST_SYNCHRO
};


class Stm
{
  public:
    LinCol  lc;
    ST      st;

    virtual~Stm(){}
            Stm(LinCol lc, ST st)
              : lc(lc), st(st)
            {}

    typedef Vector< VectorPtrTraits<Stm> > Vector;
};


class IfStm : public Stm
{
  public:
    Exp*  condExp;
    Stm*  thenStm;
    Stm*  elseStm;

   ~IfStm();
    IfStm(LinCol lc)
      : Stm(lc, ST_IF),
        condExp(0), thenStm(0), elseStm(0)
      {}
};


class WhileStm : public Stm
{
  public:
    Exp*  condExp;
    Stm*  bodyStm;

   ~WhileStm();
    WhileStm(LinCol lc)
      : Stm(lc, ST_WHILE),
        condExp(0), bodyStm(0)
      {}
};


class DoWhileStm : public Stm
{
  public:
    Exp*  condExp;
    Stm*  bodyStm;

   ~DoWhileStm();
    DoWhileStm(LinCol lc)
      : Stm(lc, ST_DO_WHILE),
        condExp(0), bodyStm(0)
      {}
};


class ForStm : public Stm
{
  public:
    Stm*  initStm;
    Exp*  condExp;
    Stm*  alterStm;
    Stm*  bodyStm;

   ~ForStm();
    ForStm(LinCol lc)
      : Stm(lc, ST_FOR),
        initStm(0), condExp(0), alterStm(0), bodyStm(0)
      {}
};


class ThrowStm : public Stm
{
  public:
    Exp*  thrownExp;

   ~ThrowStm();
    ThrowStm(LinCol lc)
      : Stm(lc, ST_THROW),
        thrownExp(0)
      {}
};


class ExpStm : public Stm
{
  public:
    Exp*  exp;

   ~ExpStm();
    ExpStm(LinCol lc, Exp* exp)
      : Stm(lc, ST_EXP),
        exp(exp)
      {}
};


class CompoStm : public Stm
{
  public:
    Stm::Vector stm;

   ~CompoStm();
    CompoStm(LinCol lc)
      : Stm(lc, ST_COMPO)
      {}
};


class EmptyStm : public Stm
{
  public:
    EmptyStm(LinCol lc)
      : Stm(lc, ST_EMPTY)
      {}
};


class ReturnStm : public Stm
{
  public:
    Exp*  returnedExp;

   ~ReturnStm();
    ReturnStm(LinCol lc, Exp* returnedExp=0)
      : Stm(lc, ST_RETURN),
        returnedExp(returnedExp)
      {}
};


class LabelStm : public Stm
{
  public:
    symbol_t  labelSym;
    Stm*      labeledStm;

   ~LabelStm();
    LabelStm(LinCol lc, symbol_t labelSym=0)
      : Stm(lc, ST_LABEL),
        labelSym(labelSym),
        labeledStm(0)
      {}
};


class CaseStm : public Stm
{
  public:
    Exp*  casedExp;

   ~CaseStm();
    CaseStm(LinCol lc, Exp* casedExp=0)
      : Stm(lc, ST_CASE),
        casedExp(casedExp)
      {}
};


class DefaultStm : public Stm
{
  public:

    DefaultStm(LinCol lc)
      : Stm(lc, ST_DEFAULT)
      {}
};


class SwitchStm : public Stm
{
  public:
    Exp*  switchedExp;
    Stm*  bodyStm;

   ~SwitchStm();
    SwitchStm(LinCol lc, Exp* switchedExp=0, Stm* bodyStm=0)
      : Stm(lc, ST_SWITCH),
        switchedExp(switchedExp), bodyStm(bodyStm)
      {}
};


class BreakStm : public Stm
{
  public:
    symbol_t  labelSym;

    BreakStm(LinCol lc, symbol_t labelSym=0)
      : Stm(lc, ST_BREAK),
        labelSym(labelSym)
      {}
};


class ContinueStm : public Stm
{
  public:
    symbol_t  labelSym;

    ContinueStm(LinCol lc, symbol_t labelSym=0)
      : Stm(lc, ST_CONTINUE),
        labelSym(labelSym)
      {}
};


class SynchroStm : public Stm
{
  public:
    Exp*  synchroExp;
    Stm*  bodyStm;

   ~SynchroStm();
    SynchroStm(LinCol lc, Exp* synchroExp=0, Stm* bodyStm=0)
      : Stm(lc, ST_SYNCHRO),
        synchroExp(synchroExp), bodyStm(bodyStm)
      {}
};

///////////////////////////////////////////////////////////////////////
// @DECL
///////////////////////////////////////////////////////////////////////

enum DT {
  DT_PARAMVAR,
  DT_LOCALVAR,
  DT_INSTVAR,
  DT_METHOD,
  DT_CTOR,
  DT_STATICBLOCK,
  DT_CLASS
};


class Decl
{
  public:
    LinCol    lc;
    DT        dt;
    symbol_t  sym;
    int       modifs;

    virtual  ~Decl(){}
              Decl(LinCol lc, DT dt, int modifs, symbol_t sym)
                : lc(lc), dt(dt), modifs(modifs), sym(sym)
                {}
    typedef ::Vector< VectorPtrTraits< Decl >  > Vector;
};


class VarDecl : public Decl
{
  public:
    TypeExp*  typeExp;
    int       xdims;   //the number of [] that follows sym.
    Exp*      initExp; //possibly null.

   ~VarDecl();
    VarDecl(LinCol lc, DT dt)
      : Decl(lc, dt, 0, 0),
        typeExp(0), xdims(0), initExp(0)
      {}

    typedef ::Vector< VectorPtrTraits< VarDecl >  > Vector;
};


class DeclStm : public Stm
{
  public:
    int             modifs;
    TypeExp*        typeExp;
    VarDecl::Vector varDecl;

    DeclStm(LinCol lc, int modifs, TypeExp* typeExp)
      : Stm(lc, ST_DECL),
        modifs(modifs), typeExp(typeExp)
      {}
};


class CatchClause {
  public:
    LinCol    lc;
    VarDecl*  param;
    Stm*      stm;

   ~CatchClause();
    CatchClause(LinCol lc)
      : lc(lc), param(0), stm(0)
      {}
    typedef Vector< VectorPtrTraits<CatchClause> > Vector;
};


class TryStm : public Stm
{
  public:
    Stm*                triedStm;
    CatchClause::Vector catchClause;
    Stm*                finallyStm; //possibly null

   ~TryStm();
    TryStm(LinCol lc)
      : Stm(lc, ST_TRY),
        triedStm(0), finallyStm(0)
      {}
};


class MethodDecl : public Decl
{
  public:
    TypeExp*              typeExp;
    VarDecl::Vector       param;
    int                   xdims;   //the number of [] that follows sym.
    ClassTypeExp::Vector  throwsClass;
    Stm*                  bodyStm;

   ~MethodDecl();
    MethodDecl(LinCol lc, DT dt, int modifs, symbol_t sym)
      : Decl(lc, dt, modifs, sym),
        typeExp(0), xdims(0), bodyStm(0)
      {}
};


class StaticBlock : public Decl
{
  public:
    Stm*  bodyStm;

   ~StaticBlock();
    StaticBlock(LinCol lc, int modifs, Stm* bodyStm=0)
      : Decl(lc, DT_STATICBLOCK, modifs, 0),
        bodyStm(bodyStm)
      {}
};


class ClassDecl : public Decl
{
  public:
    IdExp*            baseClassId;
    IdExp::Vector     interfaceId;
    Decl::Vector      decl;

   ~ClassDecl();
    ClassDecl(LinCol lc, int modifs, symbol_t sym)
      : Decl(lc, DT_CLASS, modifs, sym),
        baseClassId(0)
      {}

    void    addInterface(IdExp* interfaceId);
    void    addVar(VarDecl* var);
    void    addMethod(MethodDecl* Method);
    void    addStaticBlock(StaticBlock* block);

    typedef ::Vector< VectorPtrTraits< ClassDecl > > Vector;
};

///////////////////////////////////////////////////////////////////////
// @MODULE
///////////////////////////////////////////////////////////////////////

class Import
{
  public:
    IdExp*  id;
    bool    bPackage; // package or single class import ?

   ~Import() { delete id; }
    Import(IdExp* id, bool bPackage)
      : id(id), bPackage(bPackage)
      {}

    typedef Vector< VectorPtrTraits< Import > > Vector;
};


class Module
{
  public:
    IdExp*            packageId;
    Import::Vector    import;
    ClassDecl::Vector classDecl;

           ~Module();
            Module();
    void    addImport(IdExp* importId, bool isAll);
    void    addClassDecl(ClassDecl* classDecl);
};

//---------------------------------------------------------
}; //class AbstractSyntax
#endif //PARSER_STRUCTS_H
