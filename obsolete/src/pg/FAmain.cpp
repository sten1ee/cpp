#include "pg/fa.h"
#include "pg/re.h"
#include "pg/reread.h"
#include "pg/fatransf.h"
#include "pg/faprint.h"
#include <fstream.h>
#include <ctype.h>

//#define strnewdup(STR) strcpy(new char[1 + strlen(STR)], (STR))

void  skipWS(istream& is)
{
  while (isspace(is.peek())) {
    is.get();
  }
}


void  FA_main(int na, char** arg)
{
  ofstream fout;
  bool     redir = false;
  if (na > 1) {
    fout.open(arg[1]);
    if (!fout) {
      cerr << "Error opening file '" << arg[1] << "' !";
      return;
    }
    redir = true;
  }
  ostream& os = redir ? (ostream&)fout : (ostream&)cout;
  istream& is = cin;

/*
  re = new RE_altern(new RE_concat(new RE_char('a'),
                                   new RE_char('b')),
                     new RE_char('a'));
  re = new RE_iterat(re);
*/
  cout << "Type in some lines of the form: <reg-exp> <final-code>\n"
       << "After the last line type '~'\n";


  FA* nfa = new FA("NFA");

  while (is) {
    skipWS(is);
    if (is.peek() == '~') {
      break;
    }
    RE* re = ReadRE(is);
    skipWS(is);
    int  fc;
    if (!isdigit(is.peek())) {
      cerr << "Error: final code expected !";
      fc = -1;
    }
    else {
      is >> fc;
    }
    NFA_join_RE(nfa, re, fc);
    delete re;
  }

  PrintFA(nfa, os);
  os << "\n";

  FA* mdfa = NFA_to_MDFA(nfa);
  PrintFA(mdfa, os);
  os << "\n";

  FA* dfa = MDFA_to_DFA(mdfa);
  PrintFA(dfa, os);
  os << "\n";

  FA* omdfa = DFA_to_OMDFA(dfa);
  PrintFA(omdfa, os);
  os << "\n";

  FA* odfa = MDFA_to_DFA(omdfa);
  PrintFA(odfa, os);
  os << "\n";

  delete odfa;
  delete omdfa;
  delete dfa;
  delete mdfa;
  delete nfa;
}


void  main(int na, char** arg)
{
  FA_main(na, arg);
}
