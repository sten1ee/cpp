#include "smartop.h"
#include <assert.h>
#include <stddef.h>


enum {
  NoFix   = 0,
  PreFix  = 1,
  PostFix = 2,
  AnyFix  = 3,
};

const char  ProTerm::offs[4] = { 0,  2,  4,   6 };

ProTerm::ProTerm(const char* id)
  : id(strdup(id))
{
  leftFix = rightFix = 0;
  for (int i=0; i < 4; i++) {
    oper[i].opid = OPID(0);
    oper[i].leftPrio = oper[i].rightPrio = 0;
  }
}


Term* ProTerm::CreateInstance(void* /*auxdata*/)
{
  return new Term(*this);
}


int   ProTerm::getLeftFix(int rf) const
{
  assert(rf != NoFix);
  return ((leftFix >> offs[rf]) & 3);
}


int   ProTerm::getRightFix(int lf) const
{
  assert(lf != NoFix);
  return ((rightFix >> offs[lf]) & 3);
}


int   ProTerm::DefinedAs(TT tt)
{
  return (oper[tt].rightPrio != 0) || (oper[tt].leftPrio != 0);
}


void  ProTerm::AddTerm(TT tt, OPID opid, short lp, short rp)
{
  switch (tt) {
    case TT_Prefix :
      assert(lp == 0);
      assert(rp != 0);
      leftFix  |= (PreFix  << offs[PreFix] ) | (PreFix  << offs[AnyFix]);
      rightFix |= (PreFix  << offs[PreFix] ) | (PreFix  << offs[AnyFix]);
      break;
    case TT_Postfix:
      assert(lp != 0);
      assert(rp == 0);
      leftFix  |= (PostFix << offs[PostFix]) | (PostFix << offs[AnyFix]);
      rightFix |= (PostFix << offs[PostFix]) | (PostFix << offs[AnyFix]);
      break;
    case TT_Infix  :
      assert(lp != 0);
      assert(rp != 0);
      leftFix  |= (PostFix << offs[PreFix] ) | (PostFix << offs[AnyFix]);
      rightFix |= (PreFix  << offs[PostFix]) | (PreFix  << offs[AnyFix]);
      break;
    case TT_Term   :
      assert(lp == 0);
      assert(rp == 0);
      leftFix  |= (PreFix  << offs[PostFix]) | (PreFix  << offs[AnyFix]);
      rightFix |= (PostFix << offs[PreFix] ) | (PostFix << offs[AnyFix]);
      break;
    default: assert(0);
  }
  oper[tt].opid      = opid;
  oper[tt].leftPrio  = lp;
  oper[tt].rightPrio = rp;
}


Term::Term(ProTerm& proTerm)
  : proTerm(proTerm)
{
  prev = next = leftOp = rightOp = NULL;
  termType = TT_Unknown;
}


void  Term::setTermType(int lf, int rf)
{
  assert(lf == PreFix || lf == PostFix);
  assert(rf == PreFix || rf == PostFix);
  if (lf == PreFix)
    if (rf == PreFix)
      termType = TT_Prefix;
    else
      termType = TT_Term;
  else
    if (rf == PreFix)
      termType = TT_Infix;
    else
      termType = TT_Postfix;
}


SF  Term::SetFixes(Term* curr)
{
  int nterms;
  Term *t;
  for (nterms=0, t=curr; t != NULL; t = t->next)
    nterms++;

  int f, *fix = new int[nterms + 1], i, j, undet = 0, res = SF_Ok;

  for (i=0; i <= nterms; i++)
    fix[i] = AnyFix;
  fix[0]      = PreFix;
  fix[nterms] = PostFix;

  for (i=0; i < nterms; i++, curr = curr->next) {
    if (fix[i] != AnyFix) { //left is fixed, try to fix right:
_1_:  f = curr->getRightFix(fix[i]);
      switch (f) {
        case NoFix:
          res = SF_NoFix;
          goto EXIT;
        case AnyFix:
          if (fix[i+1] == AnyFix)
            undet = 1;
          else  //right fix is pre-set, so
            curr->setTermType(fix[i], fix[i+1]);
          break;
        default: //the whole term is fixed, but:
          if ((fix[i+1] & f) != f) {
            res = SF_NoFix;
            goto EXIT;
          }
          curr->setTermType(fix[i], fix[i+1] = f);
      }
    }
    else { //try fix left:
      fix[i] = curr->getLeftFix(fix[i+1]);
      if (fix[i] == NoFix) {
        res = SF_NoFix;
        goto EXIT;
      }
      if (fix[i] != AnyFix) { //perform back fixing:
        for (j = i, t = curr; undet > 0; undet--) {
          --j;
          t = t->prev;
          fix[j] = t->getLeftFix(fix[j+1]);
          assert(fix[j] != NoFix);
          if (fix[j] == AnyFix) {
            res = SF_AnyFix;
            goto EXIT;
          }
          t->setTermType(fix[j], fix[j+1]);
        }
        undet = 0;
        goto _1_;
      }
      else {
        undet++;
      }
    }
  }

  if (undet)
    res = AnyFix;
EXIT:
  delete[] fix;
  return (SF)res;
}


Term* Term::BuildTree(Term* curr)
{
  int r, bEqualRites = 0;
  goto _3_;
  while (1) {
    if (bEqualRites) {
      assert(curr->next);
      bEqualRites = 0;
      goto _2_;
    }
    if (curr->prev) {
      if (curr->next) {
        r = curr->prev->getRightPrio() - curr->next->getLeftPrio();
        if (r < 0) {
_1_:      curr = curr->prev;
          assert(curr->termType == TT_Prefix || curr->termType == TT_Infix);
          curr->rightOp = curr->next;
          curr->next = curr->next->next;
          if (curr->next)
            curr->next->prev = curr;
        }
        else if (r > 0) {
_2_:      curr = curr->next;
          assert(curr->termType == TT_Postfix || curr->termType == TT_Infix);
          curr->leftOp = curr->prev;
          curr->prev = curr->prev->prev;
          if (curr->prev)
            curr->prev->next = curr;
          if (curr->termType == TT_Infix) {
            do {
              curr = curr->next;
_3_:          assert(curr);
            } while (curr->termType != TT_Term);
          }
        }
        else {
          bEqualRites = 1;
          goto _1_;
        }
      }
      else
        goto _1_;
    }
    else
      if (curr->next)
        goto _2_;
      else
        return curr;//Finished
  }
}
