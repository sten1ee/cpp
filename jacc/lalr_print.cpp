#include "lalr.h"
#include "lalr_print.h"
#include <iostream>
#include <iomanip>

using std::ostream;
using std::setw;

static int  max_t_strlen;
static int  max_nt_strlen;
static int  max_tnt_strlen;
static int  max_prod_strlen;
static int  print_flags = PF_DEFAULTS;

static const char* s_tab       = "    ";
static const char* s_nltab     = "\n    ";
static const char* str_default = "$default";


struct space {
  space(int n) {
    if (n > size)
      p = begin;
    else if (n < 0)
      p = end;
    else
      p = end - n;
  }
  const char* p;
private:
  static int  size;
  static const char *begin, *end;
};
const char *space::begin = "                                                  ";
      int   space::size  = strlen(begin);
const char *space::end   = begin + size;

inline ostream& operator << (ostream& os, const space& spc)
{
  return os << spc.p;
}

struct aligned {
  const char* str;
        int   width;
  aligned(const char* s, int w) : str(s), width(w) {;}
  aligned(const Term* t, int w) : str(t->name.c_str()), width(w) {;}
};

struct t_aligned : aligned {
  t_aligned(const char* cstr) : aligned(cstr, max_t_strlen) {;}
  t_aligned(const Term* term) : aligned(term, max_t_strlen) {;}
};

struct nt_aligned : aligned {
  nt_aligned(const char* cstr) : aligned(cstr, max_nt_strlen) {;}
  nt_aligned(const Term* term) : aligned(term, max_nt_strlen) {;}
};

struct tnt_aligned : aligned {
  tnt_aligned(const char* cstr) : aligned(cstr, max_tnt_strlen) {;}
  tnt_aligned(const Term* term) : aligned(term, max_tnt_strlen) {;}
};

ostream& operator << (ostream& os, const aligned& al)
{
  return  os << al.str
             << space(al.width - strlen(al.str));
}

void  print_setf(int pf)
{
  print_flags = pf;
}

int   print_getf()
{
  return print_flags;
}

inline bool  b_print_item_la_terms()
{
  return 0 != (print_flags & PF_PRINT_ITEM_LA_TERMS);
}

inline bool  b_print_state_non_core_items()
{
  return 0 != (print_flags & PF_PRINT_STATE_NON_CORE_ITEMS);
}

inline bool  b_print_resolved_conflicts()
{
  return 0 != (print_flags & PF_PRINT_RESOLVED_CONFLICTS);
}

inline bool  b_compress_table()
{
  return 0 != (print_flags & PF_COMPRESS_TABLE);
}

// ----------------------------------------------------------------------------
// $ print_grammar
void  print_separator(ostream& os)
{
  os <<  "\n------------------------------------------------------------";
}


void  print_header(ostream& os, const char* s_header)
{
  print_separator(os);
  os << "\n  " << s_header;
  print_separator(os);
}


void  print_term(ostream& os, const Term* term)
{
  os << term->name.c_str();
}


void  print_term_set(ostream& os, const Term::set& terms)
{
  os << "{";
  for (Term::set::const_iterator i  = terms.begin();
                                 i != terms.end(); ++i) {
    if (i != terms.begin()) {
      os << " ";
    }
    print_term(os, *i);
  }
  os << "}";
}


void  verbose_print_term(ostream& os, const Term* term)
{
  os  << s_nltab << tnt_aligned(term);
  if (!term->is_terminal()) {
    os << (term->nullable ? " FIRST* "
                          : " FIRST  ");
    print_term_set(os, term->first_set);
  }
}


void  print_prod(ostream& os, const Prod* p)
{
  os << std::setw(3) << p->id << ": " << nt_aligned(p->lhs_term) << " -> ";
  for (int i = 0; i < p->rhs_terms.size(); i++) {
    os << " ";
    print_term(os, p->rhs_terms[i]);
  }
}


int  strlen(const Term* term)
{
  return  term->name.length();
}


int  strlen(const Prod* prod)
{
  int len = prod->rhs_terms.size() + 1; // for the blanks
  for (int j = 0; j < prod->rhs_terms.size(); j++) {
    len += strlen(prod->rhs_terms[j]);
  }
  return len;
}


void   recalc_max_strlens(Grammar& grammar)
{
  static const Grammar* last_recalc_grm = 0;
  if (&grammar == last_recalc_grm) {
    return; // no need to recalc !
  }
  else {
    last_recalc_grm = &grammar;
  }
  max_t_strlen = max_nt_strlen = strlen(str_default);
  for (int i = 0; i < grammar.n_terms(); i++) {
    int  term_len = strlen(grammar.term(i));
    if (grammar.term(i)->is_terminal()) {
      if (max_t_strlen < term_len) {
        max_t_strlen = term_len;
      }
    }
    else {
      if (max_nt_strlen < term_len) {
        max_nt_strlen = term_len;
      }
    }
    for (Term::prods_iterator pi  = grammar.term(i)->prods_begin(),
                              end = grammar.term(i)->prods_end();
                              pi != end; ++pi) {
      int  prod_len = strlen(*pi);
      if (max_prod_strlen < prod_len) {
        max_prod_strlen = prod_len;
      }
    }
  }
  max_tnt_strlen = max(max_t_strlen, max_nt_strlen);
}


void  print_grammar(ostream& os, Grammar& grammar)
{
  recalc_max_strlens(grammar);

  print_header(os, "Grammar");

  os << "\nTerminals are:\n";
  int i;
  for (i = 0; i < grammar.n_terms(); i++) {
    const Term* term = grammar.term(i);
    if (term->is_terminal()) {
      verbose_print_term(os, term);
    }
  }

  os << "\nNon-terminals are:\n";
  for (i = 0; i < grammar.n_terms(); i++) {
    const Term* term = grammar.term(i);
    if (!term->is_terminal()) {
      verbose_print_term(os, term);
    }
  }

  Prod::set  prods;
  prods.owns_data(false);
  for (i = 0; i < grammar.n_terms(); i++) {
    Term* term = const_cast<Term*>(grammar.term(i));
    for (Term::prods_iterator pi = term->prods_begin(), end = term->prods_end();
                              pi != end; ++pi) {
      prods.insert(*pi);
    }
  }
  os << "\nProductions are:";
  Term* lhs_prev = 0;
  for (Prod::set::iterator j  = prods.begin();
                           j != prods.end(); ++j) {
    if (lhs_prev != (*j)->lhs_term) {
      os << "\n";
      lhs_prev = (*j)->lhs_term;
    }
    os << s_nltab;
    print_prod(os, (*j));
  }
}

// ----------------------------------------------------------------------------
// $ print_LALR

void  print_LALR_item(ostream& os, LALR_item* item)
{
  os << s_nltab << nt_aligned(item->prod->lhs_term) << " -> ";
  for (int i = 0; i < item->prod->rhs_terms.size(); i++) {
    os << (i == item->rhs_pos ? '.' : ' ');
    print_term(os, item->prod->rhs_terms[i]);
  }
  os << (item->is_past() ? '.' : ' ');

  if (b_print_item_la_terms()) {
    os << space(max_prod_strlen - strlen(item->prod));

    os << s_tab << '{';
    for (Term::set::iterator j  = item->la_terms.begin();
                             j != item->la_terms.end(); ++j) {
      if (j != item->la_terms.begin()) {
        os << ' ';
      }
      print_term(os, *j);
    }
    os << '}';
  }
}


void  print_LALR_action(ostream& os, LALR_action* action)
{
  os << s_nltab << tnt_aligned(action->la_term) << s_tab;

  switch (action->getf(LALR_action::IS_FLAGS)) {
    default:
      ASSERT(0);
    case LALR_action::IS_NORMAL  :
    case LALR_action::IS_RESOLVED:
    case LALR_action::IS_CONFLICT:
    ;
  }

  ASSERT(!action->getf(LALR_action::DO_ACCEPT) ||
         (action->shift_state == 0 && action->reduce_prod == 0));

  switch (action->getf(LALR_action::DO_FLAGS)) {
    case LALR_action::DO_SHIFT:
      os << (action->la_term->is_terminal() ? "shift and goto "
                                            : "          goto ")
         << action->shift_state->id;
      break;
    case LALR_action::DO_REDUCE:
      ASSERT(action->reduce_prod->id != 0);
      ASSERT(action->la_term->is_terminal());
      os << "reduce by rule " << action->reduce_prod->id;
      break;
    case LALR_action::DO_ACCEPT:
      os << "accept";
      break;
    case LALR_action::DO_ERROR:
      os << "error: nonassoc";
      break;
    default:
      ASSERT(0);
  }

  if (action->getf(LALR_action::IS_CONFLICT)) {
    if (action->shift_state != 0 && !action->getf(LALR_action::DO_SHIFT)) {
      os << s_nltab << tnt_aligned("") << space(strlen(s_tab) - 1)
         << "|shift and goto " << action->shift_state->id;
    }
    if (action->reduce_prod != 0 && !action->getf(LALR_action::DO_REDUCE)) {
      os << s_nltab << tnt_aligned("") << space(strlen(s_tab) - 1)
         << "|reduce by rule " << action->reduce_prod->id;
    }
    if (action->rest_reduce_prods != 0) {
      for (Prod::set::iterator i  = action->rest_reduce_prods->begin();
                               i != action->rest_reduce_prods->end();
                             ++i) {
        os << s_nltab << tnt_aligned("") << space(strlen(s_tab) - 1)
           << "!reduce by rule " << (*i)->id;
      }
    }
  }
}


void  print_LALR_state(ostream& os, LALR_state* state, int def_reduce)
{
  print_separator(os);
  os << "\nState " << state->id;

  int n_printed = 0;
  for (LALR_item::set::iterator i  = state->items.begin();
                                i != state->items.end(); ++i) {
    if (!b_print_state_non_core_items()
      && n_printed++ == state->n_core_items) {
      break;
    }
    print_LALR_item(os, SURE_CAST(*i, LALR_item));
  }
  os << "\nActions:";
  for (LALR_action::set::iterator j  = state->actions.begin();
                                  j != state->actions.end(); ++j) {
    LALR_action* act = (*j);
    if (!act->getf(LALR_action::DO_REDUCE)
      || act->reduce_prod->id != def_reduce)
    print_LALR_action(os, *j);
  }
  if (0 <= def_reduce) {
    os << s_nltab << tnt_aligned(str_default) << s_tab
       << "reduce by rule " << def_reduce;
  }
}


void  print_LALR_conflict(ostream& os, LALR_action* action)
{
  if (!b_print_resolved_conflicts()
    && action->getf(LALR_action::IS_RESOLVED)) {
    return;
  }
  os << "\nin State " << setw(3) << action->host_state->id
     << " on lookahead " << t_aligned(action->la_term);

  if (action->getf(LALR_action::IS_CONFLICT)) {
    os << " :*";
  }
  if (action->getf(LALR_action::IS_RESOLVED)) {
    os << " : ";
  }

  if (action->rest_reduce_prods != 0) {
    os << " reduce-reduce";
  }
  if (action->shift_state) {
    os << " shift-reduce";
  }

  os << " resolved"
     << (action->getf(LALR_action::IS_RESOLVED) ? "" : " by default")
     << " as "
     << (action->getf(LALR_action::DO_SHIFT)  ? "shift"  :
         action->getf(LALR_action::DO_REDUCE) ? "reduce" :
         action->getf(LALR_action::DO_ERROR)  ? "error"  :
                                                "???"    );
}


void  print_LALR_table(ostream& os, LALR_table& table)
{
  if (table.grammar != 0) {
    recalc_max_strlens(*table.grammar);
  }
  print_header(os, "LALR  table");

  if (table.n_conflicts()) {
    os << "\n" << table.n_conflicts() << " conflicts found :\n";
    for (int j = 0; j < table.n_conflicts(); j++) {
      print_LALR_conflict(os, table.conflict(j));
    }
  }
  bool compress = b_compress_table();
  int* reduce_counts = (compress ? new int[table.grammar->n_prods()] : 0);
  for (int i = 0; i < table.n_states(); i++) {
    int def_reduce =
      (compress ? table.calc_default_reduce(table.state(i), reduce_counts)
                : -1);
    print_LALR_state(os, table.state(i), def_reduce);
  }
  if (compress)
    delete[] reduce_counts;
}
