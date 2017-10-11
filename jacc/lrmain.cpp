#include "lalr.h"
#include "lalr_print.h"
#include "lalr_read.h"
#include <fstream>

using std::ostream;
using std::istream;
using std::ofstream;
using std::ifstream;
using std::cin;
using std::cout;
using std::cerr;

// ----------------------------------------------------------------------------
// $ main

int  LR_main(int na, char** arg)
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
        return -1;
      }
      if (++i == na) {
        cerr << "\nError: Missing input file name after -i option !";
        return -1;
      }
      fin.open(arg[i]);
      if (!fin) {
        cerr << "\nError: Unable to open input file '" << arg[i] << "' !";
        return -1;
      }
      redir_in = true;
      in_name  = arg[i];
      continue;
    }
    // -- redir output :
    if (strcmp(arg[i], "-o") == 0) {
      if (redir_out) {
        cerr << "\nError: Duplicate cmd line opttion: -o !";
        return -1;
      }
      if (++i == na) {
        cerr << "\nError: Missing output file name after -o option !";
        return -1;
      }
      fout.open(arg[i]);
      if (!fout) {
        cerr << "\nError: Unable to open output file '" << arg[i] << "' !";
        return -1;
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
      return 0;
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
      return 0;
    }
    // -- unknow parameter/option :
    if (arg[i][0] == '-') {
      cerr << "\nError: unknown command line option: " << arg[i] << " !";
    }
    else {
      cerr << "\nError: unknown command line parameter: " << arg[i] << " !";
    }
    cerr << "\nRun with command line option -h to get help";
    return -1;
  }

  if (!opt_quiet) {
    cout << "LR - a prototype of LR1/LALR1 parser generator\n"
            "Run with command line option -h to a get brief help\n"
            "Author   : Stanislav Jordanov <stanislav.jordanov@gmail.com>\n"
            "Copyleft : jerk0main 1998, 2017\n"
            "input  is: " << in_name.c_str()  << "\n"
            "output is: " << out_name.c_str() << "\n\n";
  }

  ostream& os = redir_out ? (ostream&)fout : (ostream&)cout;
  istream& is = redir_in  ? (istream&)fin  : (istream&)cin;
  ostream& erros = (ostream&)cerr;

  Grammar grammar;
  read_grammar(is, grammar, erros);
  grammar.prepare();

  if (opt_print_grammar) {
    print_grammar(os, grammar);
  }

  if (opt_print_lr1_tab || opt_print_lalr_tab) {
    // even if it won't be printed the LALR table must be constructed:
    LALR_table   lalr_table;
    lalr_table.prepare_from(grammar);
    if (0 != lalr_table.n_conflicts()) {
      cerr << lalr_table.n_conflicts() << " conflicts found\n";
      lalr_table.resolve_conflicts();
    }
    print_LALR_table(os, lalr_table);
  }

  return 0;
}

/*****
#ifdef __BORLANDC__
int  __cdecl main(int na, char** arg)
#else
int          main(int na, char** arg)
#endif
{
  return LR_main(na, arg);
}
*****/