#include <ctype.h>
#include "smartop.h"
#include "parsetre.h"

static int  LastOPID = 0;


bool hasId(ProTerm*& pt, void* id)
{
  return (strcmp(pt->id, (char*)id) == 0);
}


int ParseProfileLine(char* str, ProTermList& ptList)
{
  int leftPrio = 0, rightPrio = 0;
  char opid[64], *pch = opid;

  while (isspace(*str))
    str++;

  while (isdigit(*str)) {
    leftPrio *= 10;
    leftPrio += (*str - '0');
    str++;
  }

  while (isspace(*str))
    str++;

  while (!isspace(*str) && !isdigit(*str) && *str != 0)
    *pch++ = *str++;

  if (pch == opid)
    return Err_NoId;

  *pch = 0;

  while (isspace(*str))
    str++;

  while (isdigit(*str)) {
    rightPrio *= 10;
    rightPrio += (*str - '0');
    str++;
  }

  while (isspace(*str))
    str++;

  if (*str != '%' && *str != 0)
    return Err_Syntax;

  //ProTerm* proTerm = ptli.firstThat(hasId, opid) ? ptli.current() : 0;
  ProTerm* proTerm = 0;
  for (ProTermListIter ptli = ptList.begin(); ptli != ptList.end(); ++ptli) {
    ProTerm* current = *ptli;
    if (strcmp(current->id, opid) == 0) {
      proTerm = current;
      break;
    }
  }

  if (!proTerm) {
    proTerm = new ProTerm(opid);
    ptList.add(proTerm);
  }
  TT tt = leftPrio ?
            (rightPrio ? TT_Infix : TT_Postfix) :
            (rightPrio ? TT_Prefix: TT_Term);
  if (proTerm->DefinedAs(tt))
    return Err_Redef;

  proTerm->AddTerm(tt, (OPID)++LastOPID, leftPrio, rightPrio);
  return OK;
}


int  Consult(std::istream& is, ProTermList& ptList)
{
  int l=0, res = OK;
  char line[256];
  while (is) {
    l++;
    is.getline(line, sizeof(line));
    char* pch = line;
    while (isspace(*pch))
      pch++;
    if (!*pch)
      continue;
    res = ParseProfileLine(line, ptList);
    if (res != OK)
      return (res | (l << 2));
  }
  return (res | (l << 2));
}


void  Order(ProTermList& inList, ProTermList& outList)
{
  while (!inList.empty()) {
    ProTermListIter  min;
    for (ProTermListIter pt = inList.begin(); pt != inList.end(); ++pt)
      if (!min.initialized() || strcmp((*pt)->id, (*min)->id) < 0)
        min = pt;
    outList.add(*min);
    inList.erase(min); //no delete
  }
}


int ConsParserTree(TopParserNode* topNode, ProTermList& ptList)
{
  for (ProTermListIter pt = ptList.begin(); pt != ptList.end(); ++pt) {
    ATRes res = topNode->AddToken((*pt)->id, *pt);
    if (res != ATOk)
      return res;
  }
  return ATOk;
}
