#if !defined(__PARSETRE_CPP)
#    define  __PARSETRE_CPP

#include "smartop.h"
#include "util/list.h"
#include "util/listt.h"
#include <iostream>
#include <ctype.h>

#define OK         0
#define Err_NoId   1
#define Err_Syntax 2
#define Err_Redef  3


typedef list< list_traits_ptr< ProTerm > >  ProTermList;
typedef ProTermList::iterator               ProTermListIter;


class ParserStream {
  public:
    ParserStream()
      : ch(' ')
    {}
    virtual void  loadChars()=0;
    char  ch;
};


class ParserIStream : public ParserStream {
  private:
      std::istream& is;
  public:
    ParserIStream(std::istream& is)
      : is(is)
    {}
    virtual void  loadChars() { ch = is.get(); }
};


class ParserNode {
  public:
                  ParserNode() { firstChild = nextSib = NULL; }
    virtual      ~ParserNode();
    virtual Term* parseTerm(ParserStream& ps)=0;
    virtual int   handlesCh(char ch)=0;
    ParserNode*   findSibHandling(char ch);
    void          AddChild(ParserNode* newChild);
  public:
    ParserNode   *firstChild, *nextSib;
};


class ChParserNode : public ParserNode {
  public:
    ChParserNode(ProTerm* proTerm, char ch)
      : proTerm(proTerm), ch(ch)
    {}
    virtual Term* parseTerm(ParserStream& ps);
    virtual int   handlesCh(char ch);
  protected:
    ProTerm      *proTerm;
    char  ch;
};


class IdentParserNode : public ParserNode {
  public:
    virtual Term* parseTerm(ParserStream& ps);
    virtual int   handlesCh(char ch) { return isalpha(ch); }
};


class NumberParserNode : public ParserNode {
  public:
    virtual Term* parseTerm(ParserStream& ps);
    virtual int   handlesCh(char ch) { return isdigit(ch); }
};


enum ATRes {
  ATOk,
  ATErrShortId,
  ATErrLongId,
};

class TopParserNode : public ParserNode {
  public:
    virtual Term* parseTerm(ParserStream& ps);
    virtual int   handlesCh(char ch);

            ATRes AddToken(const char* id, ProTerm* proTerm);
};


int   Consult(std::istream& is, ProTermList& ptList);
void  Order(ProTermList& inList, ProTermList& outList);
int   ConsParserTree(TopParserNode* topNode, ProTermList& ptList);

#endif