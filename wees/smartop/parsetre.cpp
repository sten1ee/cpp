#include "parsetre.h"
#include "initterm.h"


ParserNode::~ParserNode()
{
  if (nextSib)
    delete nextSib;
  if (firstChild)
    delete firstChild;
}


void  ParserNode::AddChild(ParserNode* newChild)
{
  assert(newChild->nextSib == NULL);
  newChild->nextSib = firstChild;
  firstChild = newChild;
}


ParserNode* ParserNode::findSibHandling(char ch)
{
  ParserNode* curr;
  for (curr = firstChild; curr != NULL; curr = curr->nextSib)
    if (curr->handlesCh(ch))
      return curr;
  return NULL;
}


int ChParserNode::handlesCh(char _ch)
{
  return (ch == _ch);
}


Term* ChParserNode::parseTerm(ParserStream& ps)
{
  assert(ps.ch == ch);
  ps.loadChars();

  ParserNode *curr;
  for (curr = firstChild; curr != NULL; curr = curr->nextSib) {
    if (curr->handlesCh(ps.ch))
      return curr->parseTerm(ps);
  }
  return proTerm->CreateInstance();
}


Term* IdentParserNode::parseTerm(ParserStream& ps)
{
  assert(isalpha(ps.ch));
  char buf[64], *pch = buf;
  while (isalpha(ps.ch)) {
    *pch++ = ps.ch;
    ps.loadChars();
  }
  *pch = 0;
  return new IdentTerm(buf);
}


Term* NumberParserNode::parseTerm(ParserStream& ps)
{
  assert(isdigit(ps.ch));
  char buf[64], *pch = buf;
  while (isdigit(ps.ch)) {
    *pch++ = ps.ch;
    ps.loadChars();
  }
  *pch = 0;
  return new NumberTerm(buf);
}


int TopParserNode::handlesCh(char ch)
{
  ParserNode* curr;
  for (curr = firstChild; curr != NULL; curr = curr->nextSib)
    if (curr->handlesCh(ch))
      return 1;
  return 0;
}


Term* TopParserNode::parseTerm(ParserStream& ps)
{
  ParserNode *curr;
  for (curr = firstChild; curr != NULL; curr = curr->nextSib) {
    if (curr->handlesCh(ps.ch))
      return curr->parseTerm(ps);
  }
  return NULL;
}


ATRes TopParserNode::AddToken(const char* id, ProTerm* proTerm)
{
  const char* pch = id;
  ParserNode* curr = this;

  assert(*id); //id shouldn't be empty string!

  for (pch = id; pch[1]; pch++) {
    if (NULL == (curr = curr->findSibHandling(*pch)))
      return ATErrShortId;
  }
  if (NULL != (curr->findSibHandling(*pch)))
    return ATErrLongId;
  curr->AddChild(new ChParserNode(proTerm, *pch));
  return ATOk;
}
