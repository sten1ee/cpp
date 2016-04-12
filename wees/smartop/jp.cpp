#include "smartop.h"
#include "parsetre.h"
#include "initterm.h"
#include <assert.h>
#include <fstream>
#include <string.h>
#include <malloc.h>
#include <ctype.h>


void  Print(Term* t, std::ostream& os)
{
  assert(t->getProTermId());
  if (t->getProTermId()[0] != 0)
    os << t->getProTermId();
  else
    os << ((UniqTerm*)t)->name;
}


int len(Term* t)
{
  assert(t->getProTermId());
  if (t->getProTermId()[0] != 0)
    return strlen(t->getProTermId());
  else
    return strlen(((UniqTerm*)t)->name);
}


void  PrintList(Term* t, std::ostream& os)
{
  if (t)
    switch (t->getTermType()) {
      case TT_Prefix:
      case TT_Term:
        os << '<';
        break;
      case TT_Postfix:
      case TT_Infix:
        os << '>';
        break;
      case TT_Unknown:
        os << '?';
        break;
      default:
        assert(0);
    }
  while (t) {
    Print(t, os);
    switch (t->getTermType()) {
      case TT_Prefix:
      case TT_Infix:
        os << '<';
        break;
      case TT_Postfix:
      case TT_Term:
        os << '>';
        break;
      case TT_Unknown:
        os << '?';
        break;
      default:
        assert(0);
    }
    t =t->next;
  }
}


class PI { //PrintItem
public:
  Term* t;
  int r;
  int a, b;
  PI(){}
  PI(const PI& pi) { *this = pi; }
  PI(Term* t, int r, int a, int b)
    : t(t), r(r), a(a), b(b)
  {}
  int operator == (const PI& o) const { return 0; }
  void operator = (const PI& pi) { memcpy(this, &pi, sizeof(PI)); }
};

typedef list< list_traits< PI > > PIList;
void    PrintOn(PI& pi, std::ostream& os, PIList& pilist, int& ccol);

void printSpc(std::ostream& os, int s)
{
  for (int i=0; i < s; i++)
    os.put(' ');
}

void  PrintOn(PI& pi, std::ostream& os, PIList& pilist, int& ccol)
{
  int l=len(pi.t);
  int pp = (pi.a+pi.b-l)/2;
  if (pp > ccol) {
    printSpc(os, pp-ccol);
    ccol = pp;
  }

  Print(pi.t, os);

  switch (pi.t->getTermType()) {
    case TT_Prefix:
      pilist.add(PI(pi.t->rightOp, pi.r+1, pi.a, pi.b));
      break;
    case TT_Postfix:
      pilist.add(PI(pi.t->leftOp, pi.r+1, pi.a, pi.b));
      break;
    case TT_Infix:
      pilist.add(PI(pi.t->leftOp,  pi.r+1, pi.a, (pi.a+pi.b)/2));
      pilist.add(PI(pi.t->rightOp, pi.r+1, (pi.a+pi.b)/2, pi.b));
      break;
    case TT_Term:
      break;
    default:
      assert(0);
  }
}


void  PrintTree(Term* top, std::ostream& os)
{
  int crow = 0, ccol;
  if (top == NULL)
    return;
  os << '\n';
  PIList list;
  list.add(PI(top, 1, 1, 60));
  while (!list.empty()) {
    PI cp = list.front();
    list.pop_front();
    if (crow < cp.r) {
      os << '\n';
      crow++;
      ccol = 1;
    }
    PrintOn(cp, os, list, ccol);
  }
}


void DelTree(Term* top)
{
  if (top == NULL)
    return;
  DelTree(top->leftOp);
  DelTree(top->rightOp);
  delete top;
}


void  eatws(std::istream& is)
{
  while (isspace(is.peek()) && is.peek() != '\n')
    is.get();
}


int Loop(TopParserNode* topNode)
{
  std::cout << "\n\nAnother try: ";
  if (std::cin.peek() == 'q')
    return 0;
  Term *first, *last = first = getTerm(topNode);
  while (last) {
    Term *tmp = last->next = getTerm(topNode);
    if (tmp)
      tmp->prev = last;
    last = last->next;
  }
  if (!first)
    return 0;
  SF res = Term::SetFixes(first);
  switch (res) {
    case SF_Ok:
      std::cout << "Result is  : ";
      break;
    case SF_NoFix:
      std::cout << "Can't set fixes due to contradiction : ";
      break;
    case SF_AnyFix:
      std::cout << "Can't set fixes due to undetermination : ";
      break;
    default:
      assert(0);
  }
  PrintList(first, std::cout);
  if (res == SF_Ok) {
    first = Term::BuildTree(first);
    PrintTree(first, std::cout);
    DelTree(first);
  }
  else {
    while (first != NULL) {
      Term* tmp = first;
      first = first->next;
      delete tmp;
    }
  }

  return 1;
}


int main(int aa, char* arg[])
{
  ProTermList ptList;
  int res;
  const char* fname;
  { // this scope block is to close the file right after it has been consulted    
    if (aa == 1) {
      fname = "OPER";
    }
    else if (aa == 2) {
      fname = arg[1];
    }
    else {
      std::cerr << "Required parameter missing (consult file name) !";
      return 1;
    }
    std::ifstream is(fname);
    if (!is) {
      std::cerr << "Couldn't open consult file '" << fname << "' !";
      return 1;
    }

    res = Consult(is, ptList);
  }

  if ((res & 3) != 0) {
    std::cerr << "Error consulting file '" << fname
              << "' line " << (res >> 2) << "\n\t";
    switch (res & 3) {
      case Err_NoId  : std::cerr << "Missing operator id"  ; break;
      case Err_Syntax: std::cerr << "Bad syntax"           ; break;
      case Err_Redef : std::cerr << "Operator redefinition"; break;
      default:
        assert(0);
    }
    return 1;
  }
  ProTermList ptOrdList;
  TopParserNode* topNode = new TopParserNode();
  Order(ptList, ptOrdList);
  res = ConsParserTree(topNode, ptOrdList);
  if (res != ATOk) {
    std::cerr << "Error building parser tree. line " << (res >> 2) << "\n\t";
    switch (res & 3) {
      case ATErrShortId : std::cerr << "Operator id is too short"; break;
      case ATErrLongId  : std::cerr << "Operator id is too long" ; break;
      default:
        assert(0);
    }
    return 1;
  }
  InitProTerms();
  topNode->AddChild(new IdentParserNode());
  topNode->AddChild(new NumberParserNode());

  try {
    while(Loop(topNode))
      ;
  }
  catch(...) {
    std::cerr << "\nUnhandled Exception !";
    std::cin.get();
  }

  delete topNode;
  //ptOrdList.flush(DEL_DATA); (destructor takes care) 
  return 0;
}


/*
void main(void)
{
  InitProTerms();
  try {
    while(Loop())
      ;
  }
  catch(...) {
    cerr << "\nUnhandled Exception !";
    cin.get();
  }
}
*/
