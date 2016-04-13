#if !defined(__SMARTOP_H)
#    define  __SMARTOP_H

#include <assert.h>
#include <malloc.h>
#include <string.h>

enum TT {      //Used to specify term properties:
  TT_Prefix  , //like ++ in ++a/b--
  TT_Postfix , //like -- in ++a/b--
  TT_Infix   , //like /  in ++a/b--
  TT_Term    , //like a and b in ++a/b--
  TT_Unknown , //not known yet
};

enum SF {   //Returned by Term::SetFixes(...):
  SF_Ok,    //stream fixed
  SF_NoFix, //can't fix due to contradiction
  SF_AnyFix,//can't fix due to undtermination
};

typedef int OPID;

class Term;

class ProTerm {
  public:
                   ProTerm(const char* id);
    virtual       ~ProTerm() { free(id); }
    virtual Term*  CreateInstance(void* auxdata=NULL);

    bool operator <  (const ProTerm& pt) const { return strcmp(id, pt.id) < 0; }
    bool operator == (const ProTerm& pt) const { return strcmp(id, pt.id) == 0; }

    void  AddTerm(TT tt, OPID opid, short leftFix, short rightFix);
    int   DefinedAs(TT tt);

    char* id;
    short leftFix, rightFix;
    struct  {
      OPID  opid;
      short leftPrio, rightPrio;
    } oper[4];

    int   getLeftFix (int rf) const;
    int   getRightFix(int lf) const;

    static  const short  offs[4];

  friend class Term;
};


class Term {
  public:
                  Term(ProTerm& proTerm);
    virtual      ~Term(){}
            TT    getTermType() const { return termType; }
    const char*   getProTermId()const { return proTerm.id; }
    static  SF    SetFixes(Term* first);
    static  Term* BuildTree(Term* first);
  private:
            int   getLeftFix (int rf) const { return proTerm.getLeftFix (rf); }
            int   getRightFix(int lf) const { return proTerm.getRightFix(lf); }
            int   getLeftPrio () const { assert(termType != TT_Unknown); return proTerm.oper[termType].leftPrio;  }
            int   getRightPrio() const { assert(termType != TT_Unknown); return proTerm.oper[termType].rightPrio; }
            void  setTermType(int lf, int rf);

  public:
    Term     *prev, *next, *leftOp, *rightOp;
    TT        termType;
    ProTerm  &proTerm;
};

#endif //smartop.h
