#include "pg/LR1.h"
#include "pg/LALR.h"
#include "pg/yacc_exp.h"
#include "util/progress.h"
#include <fstream.h>
#include <ctype.h>


int  max_term_strlen;
int  max_prod_strlen;

const char* s_tab   = "    ";
const char* s_nltab = "\n    ";

class ConProgressBar : public ProgressBar
{
    int lastStatusLen;
  public:
    void  start(const char* sJobDescription);
    void  show(const char* sJobStatus);
    void  finish();
};

void  ConProgressBar::start(const char* sJobDescription)
{
  cerr << '\n' << sJobDescription << '\t';
  lastStatusLen = 0;
}

void  ConProgressBar::show(const char* sJobStatus)
{
  for (int i = lastStatusLen; 0 < i--; ) {
    cerr << '\b';
  }
  cerr << sJobStatus;
  lastStatusLen = strlen(sJobStatus);
}

void  ConProgressBar::finish()
{
  cerr << " (done)";
}


// ----------------------------------------------------------------------------
// $ read grammar

string  readSymbol(const char*& s)
{
  while (isspace(*s))
    s++;
  const char* r = s;
  while (*s && !isspace(*s))
    s++;
  return string(r, size_t(s-r));
}


enum RP_ERR {
  RP_OK,
  RP_ERR_MISSING_LHS,
  RP_ERR_MISSING_ARROW,
  RP_ERR_MISSING_PREC_TERM,
  RP_ERR_PREC_TERM_MISSING_PREC,
  RP_ERR_NONTERM_HAS_PREC
};


RP_ERR  readProd(const char* line, Grammar& grammar)
{
  string lhsTermName = readSymbol(line);
  if (lhsTermName == "") {
    return RP_ERR_MISSING_LHS;
  }

  string  symbol = readSymbol(line);
  if (symbol != "->") {
    return RP_ERR_MISSING_ARROW;
  }

  Term* lhsTerm = grammar.termByName(lhsTermName);
  if (lhsTerm->prec != 0) {
    return RP_ERR_NONTERM_HAS_PREC;
  }
  Term::Vector  rhsTerms;
  int           prec = 0;
  while ((symbol = readSymbol(line)) != "") {
    if (symbol == "%prec") {
      symbol = readSymbol(line);
      if (symbol == "") {
        return RP_ERR_MISSING_PREC_TERM;
      }
      Term* precTerm = grammar.termByName(symbol);
      if (precTerm->prec == 0) {
        return RP_ERR_PREC_TERM_MISSING_PREC;
      }
      prec = precTerm->prec;
    }
    else {
      rhsTerms.push(grammar.termByName(symbol));
    }
  }

  if (grammar.nProds() == 0) { //no prods added so far -> no start term yet !
    grammar.addStartTerm(lhsTerm);
  }
  grammar.addProd(lhsTerm, rhsTerms, prec);

  return RP_OK;
}


bool  readGrammar(istream& is, Grammar& grammar, ostream& erros)
{
  char  lineBuf[256];
  int   lineNo = 0;
  int   prec = 0;
  while (is) {
    lineNo++;
    is.getline(lineBuf, sizeof(lineBuf));
    const char* line = lineBuf;
    //check for end of input:
    if (strcmp(line, "~") == 0) {
      break;
    }
    if (*line == '%') { //some directive
      line++;
      string  symbol = readSymbol(line);

      Term::Assoc assoc; //= Term::ASSOC_UNDEF;
      if (symbol == "left") {
        assoc = Term::ASSOC_LEFT;
      }
      else if (symbol == "right") {
        assoc = Term::ASSOC_RIGHT;
      }
      else if (symbol == "nonassoc") {
        assoc = Term::ASSOC_NONASSOC;
      }
      else {
        erros << "Error (line " << lineNo << "): Unknown directive '"
              << symbol.c_str() << "'\n";
        return false;
      }
      if (assoc != Term::ASSOC_UNDEF) {
        prec++; //advance to next prec level
        while ((symbol = readSymbol(line)) != "") {
          Term* term = grammar.termByName(symbol);
          if (term->prec != 0) {
            erros << "Error (line " << lineNo
                  << "): Prec redefinition of term '"
                  << term->name.c_str() << "'\n";
            return false;
          }
          if (!term->isTerminal()) {
            erros << "Error (line " << lineNo
                  << "): Prec defined for non-term '"
                  << term->name.c_str() << "'\n";
            return false;
          }
          term->assoc = assoc;
          term->prec  = prec;
        }
      }
      continue; //with next line
    }

    while (isspace(*line)) {
      line++;
    }
    if (*line == 0) {
      continue;
    }

    switch (readProd(line, grammar)) {
      case RP_OK:
        break;
      case RP_ERR_MISSING_LHS:
        erros << "Error (line " << lineNo << "): Production missing LHS\n";
        return false;
      case RP_ERR_MISSING_ARROW:
        erros << "Error (line " << lineNo << "): Production missing '->'\n";
        return false;
      case RP_ERR_MISSING_PREC_TERM:
        erros << "Error (line " << lineNo << "): Missing term after %prec\n";
        return false;
      case RP_ERR_PREC_TERM_MISSING_PREC:
        erros << "Error (line " << lineNo << "): Precless term after %prec\n";
        return false;
      case RP_ERR_NONTERM_HAS_PREC:
        erros << "Error (line " << lineNo << "): Non-term has prec\n";
        return false;
      default:
        ASSERT(0);
    }
  }
  return true;
}

// ----------------------------------------------------------------------------
// $ print_grammar
void  print_separator(ostream& os)
{
  os <<  "\n------------------------------------------------------------";
}


void  print_header(ostream& os, const char* sHeader)
{
  print_separator(os);
  os << "\n  " << sHeader;
  print_separator(os);
}


struct space {
  int n;
  space(int n) : n(n) {}
};

ostream& operator << (ostream& os, const space& spc)
{
  int n = spc.n;
  while (0 < n--) os << ' ';
  return os;
}

struct aligned {
  const Term* term;
  aligned(const Term* term) : term(term) {}
};

ostream& operator << (ostream& os, const aligned& al)
{
  return  os << al.term->name.c_str()
             << space(max_term_strlen - al.term->name.length());
}

void  print_term(ostream& os, const Term* term)
{
  os << term->name.c_str();
}


void  print_term_set(ostream& os, const Term::Set& terms)
{
  os << "{";
  for (int i = 0; i < terms.size(); i++) {
    if (i) {
      os << " ";
    }
    print_term(os, terms[i]);
  }
  os << "}";
}


void  verbose_print_term(ostream& os, const Term* term)
{
  os  << s_nltab << aligned(term)
      <<                    "   FOLLOW  ";
  print_term_set(os, term->followSet);

  if (!term->isTerminal()) {
    os << s_nltab << space(max_term_strlen)
       <<                   "   FIRST   ";
    print_term_set(os, term->firstSet);
    if (term->nullable) {
      os << s_nltab << space(max_term_strlen)
         <<                 "   NULLABLE";
    }
  }
}


void  print_prod(ostream& os, const Prod* p)
{
  os << (p->id < 10 ? "  " : p->id < 100 ? " " : "");
  os << p->id << ": " << aligned(p->lhsTerm) << " -> ";
  for (int i = 0; i < p->rhsTerms.size(); i++) {
    os << " ";
    print_term(os, p->rhsTerms[i]);
  }
}


int  strlen(const Term* term)
{
  return  term->name.length();
}


int  strlen(const Prod* prod)
{
  int len = prod->rhsTerms.size() + 1; // for the blanks
  for (int j = 0; j < prod->rhsTerms.size(); j++) {
    len += strlen(prod->rhsTerms[j]);
  }
  return len;
}


void   recalc_max_strlens(Grammar& grammar)
{
  for (int i = 0; i < grammar.nTerms(); i++) {
    int  term_len = strlen(grammar.term(i));
    if (max_term_strlen < term_len) {
      max_term_strlen = term_len;
    }
    for (ProdNode* p = grammar.term(i)->prodFirst; p; p = p->prodNext) {
      int  prod_len = strlen(p);
      if (max_prod_strlen < prod_len) {
        max_prod_strlen = prod_len;
      }
    }
  }
}


void  print_grammar(ostream& os, Grammar& grammar)
{
  int i;

  print_header(os, "Grammar");

  os << "\nTerminals are:\n";
  for (i = 0; i < grammar.nTerms(); i++) {
    const Term* term = grammar.term(i);
    if (term->isTerminal()) {
      verbose_print_term(os, term);
    }
  }

  os << "\nNon-terminals are:\n";
  for (i = 0; i < grammar.nTerms(); i++) {
    const Term* term = grammar.term(i);
    if (!term->isTerminal()) {
      verbose_print_term(os, term);
    }
  }

  Prod::Set  prods(grammar.nProds(), NO_AUTO_INCREMENT);
  prods.ownsData(false);
  for (i = 0; i < grammar.nTerms(); i++) {
    const Term* term = grammar.term(i);
    for (const ProdNode* p = term->prodFirst; p != 0; p = p->prodNext) {
      prods.insert(const_cast<ProdNode*>(p));
    }
  }
  os << "\nProductions are:";
  Term* lhsPrev = 0;
  for (int j = 0; j < prods.size(); j++) {
    if (lhsPrev != prods[j]->lhsTerm) {
      os << "\n";
      lhsPrev = prods[j]->lhsTerm;
    }
    os << s_nltab;
    print_prod(os, prods[j]);
  }
}

// ----------------------------------------------------------------------------
// $ print_LR1

void  print_LR1_spot(ostream& os, LR1_spot* spot)
{
  os << s_nltab << aligned(spot->prod->lhsTerm) << " -> ";
  for (int i = 0; i < spot->prod->rhsTerms.size(); i++) {
    os << (i == spot->rhsPos ? '.' : ' ');
    print_term(os, spot->prod->rhsTerms[i]);
  }
  os << (spot->isPast() ? '.' : ' ');

  os << space(max_prod_strlen - strlen(spot->prod));

  os << s_tab << '{';
  for (int j = 0; j < spot->laTerms.size(); j++) {
    if (j) {
      os << ' ';
    }
    print_term(os, spot->laTerms[j]);
  }
  os << '}';
}


void  print_LR1_action(ostream& os, LR1_action* action)
{
  os << s_nltab << aligned(action->laTerm);
  int nact = 0;
  if (action->shiftState) {
    nact++;
    os << s_tab << "shift and goto " << action->shiftState->id;
  }
  for (int k = 0; k < action->reduceProds.size(); k++) {
    nact++;
    Prod* prod = action->reduceProds[k];
    ASSERT(prod->id);
    os << s_tab << "reduce by rule " << prod->id;
  }
  if (action->accept) {
    nact++;
    os << s_tab << "accept";
  }
  if (nact == 0) {
    os << s_tab << "error (nonassociative)";
  }
}


void  print_LR1_state(ostream& os, LR1_state* state)
{
  print_separator(os);
  os << "\nState " << state->id;
  if (state->redu_state != 0) {
    os << "  (R " << state->redu_state->id << ")";
  }

  for (int i = 0; i < state->spots.size(); i++) {
    print_LR1_spot(os, state->spots[i]);
  }
  os << "\nActions:";
  for (int j = 0; j < state->actions.size(); j++) {
    print_LR1_action(os, state->actions[j]);
  }
}


void  print_LR1_conflict(ostream& os, LR1_conflict* conflict)
{
  os << "in State " << conflict->state->id
     << " on lookahead ";
  print_term(os, conflict->action->laTerm);
  os << " : ";
  if (conflict->type & LR1_conflict::SR) {
    os << "Shift-";
  }
  if (conflict->type & LR1_conflict::AR) {
    os << "Accept-";
  }
  if (conflict->type & LR1_conflict::RR) {
    os << "Reduce-";
  }
  os << "Reduce";
}


void  print_LR1_table(ostream& os, LR1_table& table)
{
  if (&table.grammar != 0) {
    print_header(os, "LR1  table");
  }
  else {
    print_header(os, "LALR  table");
  }

  if (table.nConflicts()) {
    os << "\n" << table.nConflicts() << " conflicts found :\n";
    for (int j = 0; j < table.nConflicts(); j++) {
      os << s_nltab;
      print_LR1_conflict(os, table.conflict(j));
    }
  }
  for (int i = 0; i < table.nStates(); i++) {
    print_LR1_state(os, table.state(i));
  }
}

// ----------------------------------------------------------------------------
// $ main

void  LR_main(int na, char** arg)
{
  ofstream fout;
  string   out_name  = "stdout";
  bool     redir_out = false;

  ifstream fin;
  string   in_name   = "stdin";
  bool     redir_in  = false;

  bool     opt_quiet         = false;
  bool     opt_print_grammar = true;
  bool     opt_print_lr1_tab = true;
  bool     opt_print_lalr_tab= true;
  bool     opt_yacc_exp      = false;

  for (int i = 1; i < na; i++) {
    // -- redir input :
    if (strcmp(arg[i], "-i") == 0) {
      if (redir_in) {
        cerr << "\nWarning: Duplicate cmd line option: -i ! (ignored)";
        return;
      }
      if (++i == na) {
        cerr << "\nError: Missing input file name after -i option !";
        return;
      }
      fin.open(arg[i]);
      if (!fin) {
        cerr << "\nError: Unable to open input file '" << arg[i] << "' !";
        return;
      }
      redir_in = true;
      in_name  = arg[i];
      continue;
    }
    // -- redir output :
    if (strcmp(arg[i], "-o") == 0) {
      if (redir_out) {
        cerr << "\nWarning: Duplicate cmd line opttion: -o ! (ignored)";
        return;
      }
      if (++i == na) {
        cerr << "\nError: Missing output file name after -o option !";
        return;
      }
      fout.open(arg[i]);
      if (!fout) {
        cerr << "\nError: Unable to open output file '" << arg[i] << "' !";
        return;
      }
      redir_out = true;
      out_name  = arg[i];
      continue;
    }
    // -- quiet mode :
    if (strcmp(arg[i], "-q") == 0) {
      opt_quiet = true;
      continue;
    }
    // -- print grammar :
    if (strcmp(arg[i], "-grm") == 0) {
      opt_print_grammar = true;
      continue;
    }
    if (strcmp(arg[i], "-no-grm") == 0) {
      opt_print_grammar = false;
      continue;
    }
    // -- print lr1 :
    if (strcmp(arg[i], "-lr1") == 0) {
      opt_print_lr1_tab = true;
      continue;
    }
    if (strcmp(arg[i], "-no-lr1") == 0) {
      opt_print_lr1_tab = false;
      continue;
    }
    // -- print lalr :
    if (strcmp(arg[i], "-lalr") == 0) {
      opt_print_lalr_tab = true;
      continue;
    }
    if (strcmp(arg[i], "-no-lalr") == 0) {
      opt_print_lalr_tab = false;
      continue;
    }
    if (strcmp(arg[i], "-yacc-exp") == 0) {
      opt_yacc_exp = true;
      continue;
    }
    if (strcmp(arg[i], "-d") == 0) {
      cout << "\n... This program is dedicated to Joanna Johan Davidova"
              "\n    for her sometimes wounding but always hot love ...\n";
      continue;
    }
    // -- print help on input file format:
    if (strcmp(arg[i], "-hi") == 0) {
      cout <<
      "The input is a text file;\n"
      "Each line of it is blank or contains a grammar production of the form:\n"
      "<sym> -> <sym1> <sym2> ... <symN>\n"
      "(zero or more sym(s) on the right hand side of the production)\n"
      "EOF or single line containing only '~' is considered for end-of-input\n"
      "Sample input:\n"
      "E -> E + T\n"
      "E -> E - T\n"
      "E -> T\n"
      "\n"
      "T -> T * F\n"
      "T -> T / F\n"
      "T -> F\n"
      "\n"
      "F -> + F\n"
      "F -> - F\n"
      "F -> id\n"
      "F -> num\n"
      "F -> ( E )\n"
      "~";
      return;
    }
    // -- print help on command line options:
    if (strcmp(arg[i], "-h") == 0) {
      cout <<
      "LR - a prototype of LR1/LALR1 parser generator\n"
      "Author  : Stanislav Jordanov <stenly@sirma.bg400.bg>\n"
      "Copyleft: jerk0main 1998\n"
      "Command line options:\n"
      "\t-i <input-file-name>  - redirect input  (def = stdin)\n"
      "\t-o <output-file-name> - redirect output (def = stdout)\n"
      "\t-q                    - quiet mode; don't print logo\n"
      "\t-grm / -no-grm        - print (def) / not print grammar\n"
      "\t-lr1 / -no-lr1        - print (def) / not print LR1 table\n"
      "\t-lalr/ -no-lalr       - print (def) / not print LALR table\n"
      "\t-yacc-exp             - export grammar in yacc format and return\n"
      "\t-d                    - dedication ...\n"
      "\t-hi                   - print help on input file format and exit\n"
      "\t-h                    - print this help message to stdout and exit\n"
      ;
      return;
    }
    // -- unknow parameter/option :
    if (arg[i][0] == '-') {
      cerr << "\nError: unknown command line option: " << arg[i] << " !";
    }
    else {
      cerr << "\nError: unknown command line parameter: " << arg[i] << " !";
    }
    cerr << "\nRun with command line option -h to get help";
    return;
  }

  if (!opt_quiet) {
    cout << "LR - a prototype of LR1/LALR1 parser generator\n"
            "Run with command line option -h to a get brief help\n"
            "Author   : Stanislav Jordanov <stenly@sirma.bg400.bg>\n"
            "Copyleft : jerk0main 1998\n"
            "input  is: " << in_name.c_str()  << "\n"
            "output is: " << out_name.c_str() << "\n\n";
  }

  ostream& os = redir_out ? (ostream&)fout : (ostream&)cout;
  istream& is = redir_in  ? (istream&)fin  : (istream&)cin;
  ostream& erros = cerr;

  Grammar grammar;
  readGrammar(is, grammar, erros);
  grammar.Prepare();

  if (opt_yacc_exp) {
    yacc_export(os, grammar);
    return;
  }
  if (opt_print_grammar || opt_print_lr1_tab || opt_print_lalr_tab) {
    recalc_max_strlens(grammar);
  }
  if (opt_print_grammar) {
    print_grammar(os, grammar);
  }

  if (opt_print_lr1_tab || opt_print_lalr_tab) {
    ConProgressBar  cpb;
    // even if it won't be printed the LR1 table must be constructed:
    LR1_table   lr1_table(grammar, cpb);
    if (opt_print_lalr_tab) {
      // construct the LALR table before printing the LR1 one
      // or else the LR1 to LALR state maping won't be available:
      LALR_table  lalr_table(lr1_table, cpb);
      print_LR1_table(os, lr1_table);
      print_LR1_table(os, lalr_table);
    }
    else {
      print_LR1_table(os, lr1_table);
    }
  }
}

#ifdef __BORLANDC__
void  __cdecl main(int na, char** arg)
#else
void          main(int na, char** arg)
#endif
{
  LR_main(na, arg);
}
