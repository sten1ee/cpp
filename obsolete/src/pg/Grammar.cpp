#include "pg/grammar.h"


void  Grammar::calcNullables()
{
  bool  reloop = true;
  while (reloop) {
    reloop = false;
    for (int i = terms.size(); 0 < i--; ) {
      Term& t = *terms[i];
      if (t.isTerminal()) { //skip terminals
        continue;
      }
      if (!t.nullable) {
        for (ProdNode* p = t.prodFirst; p != 0; p = p->prodNext) {
          bool  nullable = true;
          for (int i = p->rhsTerms.size(); 0 < i--; ) {
            if (!p->rhsTerms[i]->nullable) {
              nullable = false;
              break;
            }
          }
          if (nullable) {
            t.nullable = true;
            reloop = true;
            break;
          }
        }
      }
    }
  }
}


void  Grammar::calcFirstSets()
{
  for (int i = terms.size(); 0 < i--; ) {
    Term* term = terms[i];
    if (term->isTerminal()) {
      term->firstSet.insert(term);
    }
  }
  bool  reloop = true;
  while (reloop) {
    reloop = false;
    for (int i = terms.size(); 0 < i--; ) {
      Term& t = *terms[i];
      if (t.isTerminal()) { //skip terminals
        continue;
      }
      for (ProdNode* p = t.prodFirst; p != 0; p = p->prodNext) {
        for (int j = 0; j < p->rhsTerms.size(); j++) {
          Term& tj = *p->rhsTerms[j];
          if (!isSubset(tj.firstSet, t.firstSet)) {
            reloop = true;
            t.firstSet = t.firstSet | tj.firstSet;
          }
          if (!tj.nullable) {
            break;
          }
        }
      }
    }
  }
}


void  Grammar::calcFollowSets()
{
  bool  reloop = true;
  while (reloop) {
    reloop = false;
    for (int i = terms.size(); 0 < i--; ) {
      Term& t = *terms[i];
      if (t.isTerminal()) { //skip terminals
        continue;
      }
      for (ProdNode* p = t.prodFirst; p != 0; p = p->prodNext) {
        bool  breaked = false;
        for (int i = p->rhsTerms.size(); 0 < i--; ) {
          Term& ti = *p->rhsTerms[i];
          if (!breaked && !isSubset(t.followSet, ti.followSet)) {
            reloop = true;
            ti.followSet = ti.followSet | t.followSet;
          }
          if (!ti.nullable) {
            breaked = true;
          }
          for (int j = i; ++j < p->rhsTerms.size(); ) {
            Term& tj = *p->rhsTerms[j];
            if (!isSubset(tj.firstSet, ti.followSet)) {
              reloop = true;
              ti.followSet = ti.followSet | tj.firstSet;
            }
            if (!tj.nullable) {
              break;
            }
          }
        }
      }
    }
  }
}


int  Prod::deduce_prec() const
{
  for (int i = rhsTerms.size(); 0 < i--; ) {
    if (rhsTerms[i]->prec) {
      return  rhsTerms[i]->prec;
    }
  }
  return 0;
}


Term::~Term()
{
  while (prodFirst) {
    ProdNode* prodDel = prodFirst;
    prodFirst = prodFirst->prodNext;
    delete prodDel;
  }
}


Term::Term(int _id, const string& _name)
  : id(_id), name(_name), prodFirst(0), nullable(false),
    assoc(ASSOC_UNDEF), prec(0)
{
  firstSet.ownsData(false);
  followSet.ownsData(false);
}


Prod::Prod(int _id, Term* _lhsTerm, Term::Vector& _rhsTerms, int _prec)
  : id(_id), lhsTerm(_lhsTerm), rhsTerms(_rhsTerms), prec(_prec)
{
  rhsTerms.ownsData(false);
}


ProdNode::ProdNode(int _id, Term* _lhsTerm, Term::Vector& _rhsTerms, int prec,
                   ProdNode* _prodNext)
  : Prod(_id, _lhsTerm, _rhsTerms, prec),
    prodNext(_prodNext)
{
}


Term* Grammar::termByName(const string& name)
{
  // the # of terms is small so don't hesitate to search linear:
  for (int i = terms.size(); 0 < i--; ) {
    if (terms[i]->name == name) {
      return  terms[i];
    }
  }
  Term* newTerm = new Term(terms.size(), name);
  terms.push(newTerm);
  return  newTerm;
}


void  Grammar::addStartTerm(Term* startTerm)
{
  Term::Vector  tv;
  tv.push(startTerm);
  tv.push(term(EOF_TERM));
  Term* iniTerm = term(INI_TERM);
  iniTerm->prodFirst = new ProdNode(n_prods++, iniTerm, tv, 0, //precedenece
                                    iniTerm->prodFirst);
}


void  Grammar::addProd(Term* lhsTerm, Term::Vector& rhsTerms, int prec)
{
  lhsTerm->prodFirst = new ProdNode(n_prods++, lhsTerm, rhsTerms, prec,
                                    lhsTerm->prodFirst);
}


Grammar::Grammar()
{
  termByName("S'");  //INI_TERM
  termByName("error"); //ERR_TERM
  termByName("$");   //EOF_TERM
  n_prods = 0;
}


void  Grammar::Prepare()
{
  calcNullables();
  calcFirstSets();
  calcFollowSets();
}
