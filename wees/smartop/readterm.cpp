#include "initterm.h"
#include "parsetre.h"
#include <iostream>
#include <ctype.h>

ParserIStream pis(std::cin);

Term* getTerm(TopParserNode* topNode)
{
  while (isspace(pis.ch)) {
    if (pis.ch == '\n') {
      pis.ch = ' ';
      return NULL;
    }
    pis.loadChars();
  }
  return topNode->parseTerm(pis);
}

/*
static int  nextch = ' ', curch = ' ';
istream& is = cin;

inline
void loadChars()
{
//  do {
//    switch (nextch) {
//      case '\n':
//        lincol.inc_line();
//        lincol.set_col(0);
//        break;
//      case '\t':
//        lincol.add_col(tabWidth);
//        break;
//      case EOF:
//        curch = 0xFF;
//        return;
//      default:
//        lincol.inc_col();
//    }
    curch  = nextch;
    nextch = is.get();
//  } while( isspace(curch) );

//  outlex.lincol() = lincol;  // store the begining of the lex;
}

Term* readIdent()
{
  char buf[256], *ptr = buf;
  *ptr++ = curch;
  while (isalpha(nextch) || nextch == '_') {
    loadChars();
    *ptr++ = curch;
  }
  *ptr++ = 0;
  return new IdentTerm(buf);
}


Term* readNumber()
{
  char buf[256], *ptr = buf;
  *ptr++ = curch;
  while (isdigit(nextch) || nextch == '.') {
    loadChars();
    *ptr++ = curch;
  }
  *ptr++ = 0;
  return new NumberTerm(buf);
}


Term* getTerm()
{

  do {
    if (nextch == '\n') {
      nextch = ' ';
      return NULL;
    }
    else
      loadChars();
  }
  while (isspace(curch));

  switch (curch) {
    case EOF:
      return NULL;
    case '(':
      return new Term(ptOpenBrek);
    case ')':
      return new Term(ptClosBrek);
    case '[':
      return new Term(ptOpenKvadBrek);
    case ']':
      return new Term(ptClosKvadBrek);
    case '~':
      return new Term(ptBitNot);
    case ',':
      return new Term(ptComma);
    case '.':
      return new Term(ptDot);
    case ':':
      return new Term(ptDblDot);
    case '?':
      return new Term(ptQMark);
    case '+' :
      switch( nextch ) {
        case '+' :
          loadChars();
          return new Term(ptInc);
        case '=' :
          loadChars();
          return new Term(ptAddAsg);
        default :
          return new Term(ptAdd);
        }

    case '-' :
      switch( nextch ) {
        case '-' :
          loadChars();
          return new Term(ptDec);
        case '>' :
          loadChars();
          return new Term(ptArrow);
        case '=' :
          loadChars();
          return new Term(ptSubAsg);
        default :
          return new Term(ptSub);
        }

    case '*' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptMulAsg);
        default :
          return new Term(ptMul);
        }

    case '/' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptDivAsg);
        default :
          return new Term(ptDiv);
      }

    case '%' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptModAsg);
        default :
          return new Term(ptMod);
      }

    case '<' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptLessEq);
        case '<' :         // << so far
          loadChars();
          switch( nextch ) {
            case '=' :
              loadChars();
              return new Term(ptShLAsg);
            default :
              return new Term(ptShL);
          }
        default :
          return new Term(ptLess);
      }

    case '>' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptGrtrEq);
        case '>' :         // >> so far
          loadChars();
          switch( nextch ) {
            case '=' :
              loadChars();
              return new Term(ptShRAsg);
            default :
              return new Term(ptShR);
          }
        default :
          return new Term(ptGrtr);
      }

    case '=' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptEq);
        default :
          return new Term(ptAsg);
      }

    case '!' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptNotEq);
        default :
          return new Term(ptLogNot);
      }

    case '&' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptAndAsg);
        case '&' :
          loadChars();
          return new Term(ptLogAnd);
        default :
          return new Term(ptBitAnd);
      }

    case '|' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptOrAsg);
        case '|' :
          loadChars();
          return new Term(ptLogOr);
        default :
          return new Term(ptBitOr);
      }

    case '^' :
      switch( nextch ) {
        case '=' :
          loadChars();
          return new Term(ptXorAsg);
        default :
          return new Term(ptBitXor);
      }


    default :
      if( isalpha(curch) || curch == '_' )
        return readIdent();
      if( isdigit(curch) )
        return readNumber();
      return NULL;
  }
}
*/
