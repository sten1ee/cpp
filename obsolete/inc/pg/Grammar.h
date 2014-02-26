#ifndef PG_GRAMMAR_H
#define PG_GRAMMAR_H

#include "util/vector.h"
#include "util/vectort.h"
#include "util/sortvec.h"
#include "util/sortvect.h"
#include "util/setoper.h"
#include "util/string.h"


class Term;
class Prod;
class ProdNode;
class Grammar;

//typedef Vector< VectorTraits< string > >        StringVector;
//typedef Vector< VectorTraits< StringVector > >  StringVectorVector;

class Term
{
  public:
    int       id;
    string    name;
    ProdNode* prodFirst;
    bool      nullable;

    enum  Assoc { ASSOC_UNDEF, ASSOC_LEFT, ASSOC_RIGHT, ASSOC_NONASSOC };
    Assoc     assoc;
    int       prec; //precedence (== 0 if not set, > 0 if set)

    bool  operator == (const Term& t) const { return this == &t; }
    bool  operator <  (const Term& t) const { return id < t.id;  }

    typedef Vector< VectorPtrTraits< Term > > Vector;
    typedef SortedVector< SortVecPtrTraits< Term > > Set;

    Term::Set firstSet;
    Term::Set followSet;

    virtual  ~Term();
              Term(int _id, const string& _name);

    bool      isTerminal() const { return (prodFirst == 0); }
};


class Prod
{
  public:
    int           id;
    Term*         lhsTerm;
    Term::Vector  rhsTerms;
    int           prec; //precedence (== 0 if not set, > 0 if set)

    int   deduce_prec() const;

    bool  operator == (const Prod& p) const { return this == &p; }
    bool  operator <  (const Prod& p) const { return id < p.id;  }

    typedef  SortedVector< SortVecPtrTraits< Prod > >  Set;

    Prod(int id, Term* lhsTerm, Term::Vector& rhsTerms, int prec);
};


class ProdNode : public Prod
{
  public:
    ProdNode*     prodNext;

    ProdNode(int id, Term* lhsTerm, Term::Vector& rhsTerms, int prec,
             ProdNode* prodNext);
};


class Grammar
{
    Term::Vector  terms;
    int           n_prods;

    void  calcNullables();
    void  calcFirstSets();
    void  calcFollowSets();

  public:
          Grammar();

    Term* termByName(const string& name);
    void  addStartTerm(Term* startTerm);
    void  addProd(Term* lhsTerm, Term::Vector& rhsTerms, int prec=0);

    void  Prepare();

    enum  { INI_TERM, ERR_TERM, EOF_TERM };

    int   nTerms() const { return terms.size(); }
    Term* term(int i)    { return terms[i];     }
    int   nProds() const { return n_prods;      }
};


#endif//PG_GRAMMAR_H
