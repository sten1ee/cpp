#include "lalr.h"
#include <ctype.h>
#include <iostream>

// ----------------------------------------------------------------------------
// $ read grammar

string  read_symbol(const char*& s)
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


RP_ERR  read_prod(const char* line, Grammar& grammar)
{
  string lhs_term_name = read_symbol(line);
  if (lhs_term_name == "") {
    return RP_ERR_MISSING_LHS;
  }

  string  symbol = read_symbol(line);
  if (symbol != "->") {
    return RP_ERR_MISSING_ARROW;
  }

  Term* lhs_term = grammar.term_by_name(lhs_term_name);
//if (lhs_term->prec != 0) {
//  return RP_ERR_NONTERM_HAS_PREC;
//}
  Term::array   rhs_terms;
  Prio          prio;
  while ((symbol = read_symbol(line)) != "") {
    if (symbol == "%prec") {
      symbol = read_symbol(line);
      if (symbol == "") {
        return RP_ERR_MISSING_PREC_TERM;
      }
      Term* prec_term = grammar.term_by_name(symbol);
      if (prec_term->prio.is_null()) {
        return RP_ERR_PREC_TERM_MISSING_PREC;
      }
      prio = prec_term->prio;
    }
    else {
      rhs_terms.push(grammar.term_by_name(symbol));
    }
  }

  if (grammar.n_prods() == 0) { //no prods added so far -> no start term yet !
    grammar.add_start_term(lhs_term);
  }

  Prod* prod = new Prod(lhs_term, rhs_terms);
  if (!prio.is_null()) {
    prod->prio = prio;
  }

  grammar.add_prod(prod);

  return RP_OK;
}


bool  read_grammar(std::istream& is, Grammar& grammar, std::ostream& erros)
{
  char  line_buf[256];
  int   line_no = 0;
  int   prec = 0;
  while (is) {
    line_no++;
    is.getline(line_buf, sizeof(line_buf));
    const char* line = line_buf;
    //check for end of input:
    if (strcmp(line, "~") == 0) {
      break;
    }
    if (*line == '%') { //some directive
      line++;
      string  symbol = read_symbol(line);

      assoc_t assoc;
      if (symbol == "left") {
        assoc = ASSOC_LEFT;
      }
      else if (symbol == "right") {
        assoc = ASSOC_RIGHT;
      }
      else if (symbol == "nonassoc") {
        assoc = ASSOC_NONE;
      }
      else {
        erros << "Error (line " << line_no << "): Unknown directive '"
              << symbol.c_str() << "'\n";
        return false;
      }
      Prio  prio(++prec, assoc);
      while ((symbol = read_symbol(line)) != "") {
        Term* term = grammar.term_by_name(symbol);
        if (!term->prio.is_null()) {
          erros << "Error (line " << line_no
                << "): Prio redefinition of term '"
                << term->name.c_str() << "'\n";
          return false;
        }
/*******if (!term->is_terminal()) {
          erros << "Error (line " << line_no
                << "): Prec defined for non-term '"
                << term->name.c_str() << "'\n";
          return false;
        }*******/
        term->prio = prio;
      }
      continue; //with next line
    }

    while (isspace(*line)) {
      line++;
    }
    if (*line == 0) {
      continue;
    }

    switch (read_prod(line, grammar)) {
      case RP_OK:
        break;
      case RP_ERR_MISSING_LHS:
        erros << "Error (line " << line_no << "): Production missing LHS\n";
        return false;
      case RP_ERR_MISSING_ARROW:
        erros << "Error (line " << line_no << "): Production missing '->'\n";
        return false;
      case RP_ERR_MISSING_PREC_TERM:
        erros << "Error (line " << line_no << "): Missing term after %prec\n";
        return false;
      case RP_ERR_PREC_TERM_MISSING_PREC:
        erros << "Error (line " << line_no << "): Precless term after %prec\n";
        return false;
      case RP_ERR_NONTERM_HAS_PREC:
        erros << "Error (line " << line_no << "): Non-term has prec\n";
        return false;
      default:
        ASSERT(0);
    }
  }
  return true;
}
