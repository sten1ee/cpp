#include "grammar.h"


void  Grammar::calc_nullables()
{
  bool  reloop = true;
  while (reloop) {
    reloop = false;
    for (int i = terms.size(); 0 < i--; ) {
      Term& t = *terms[i];
      if (t.is_terminal()) { //skip terminals
        continue;
      }
      if (!t.nullable) {
        for (Term::prods_iterator pi = t.prods_begin(), end = t.prods_end();
                                  pi != end; ++pi) {
          Prod* p = *pi;
          bool  nullable = true;
          for (int i = p->rhs_terms.size(); 0 < i--; ) {
            if (!p->rhs_terms[i]->nullable) {
              nullable = false;
              break;
            }
          }
          if (nullable) {
            t.nullable = true;
            reloop = true;
            break;
          }
        }
      }
    }
  }
}


void  Grammar::calc_first_sets()
{
  for (int i = terms.size(); 0 < i--; ) {
    Term* term = terms[i];
    if (term->is_terminal()) {
      term->first_set.insert(term);
    }
  }
  bool  reloop = true;
  while (reloop) {
    reloop = false;
    for (int i = terms.size(); 0 < i--; ) {
      Term& t = *terms[i];
      if (t.is_terminal()) { //skip terminals
        continue;
      }
      for (Term::prods_iterator pi = t.prods_begin(), end = t.prods_end();
                                pi != end; ++pi) {
        Prod* p = *pi;
        for (int j = 0; j < p->rhs_terms.size(); j++) {
          Term& tj = *p->rhs_terms[j];
          if (!is_subset(tj.first_set, t.first_set)) {
            reloop = true;
            t.first_set = t.first_set | tj.first_set;
          }
          if (!tj.nullable) {
            break;
          }
        }
      }
    }
  }
}


Term::Term(int _id, const string& _name)
  : id(_id), name(_name), nullable(false)
{
  first_set.owns_data(false);
}


Prod::Prod(Term* _lhs_term, Term::array& _rhs_terms)
  : id(-1), lhs_term(_lhs_term),
    rhs_terms(_rhs_terms.begin(), _rhs_terms.size())
{
}

Term* Prod::get_rightmost_prio_term() const
{
  for (int i = rhs_terms.size(); 0 < i--; ) {
    if (!rhs_terms[i]->prio.is_null()) {
      return rhs_terms[i];
    }
  }
  return 0;
}

Term* Grammar::term_by_name(const string& name)
{
  // the # of terms is small so don't hesitate to search linear:
  for (int i = terms.size(); 0 < i--; ) {
    if (terms[i]->name == name) {
      return  terms[i];
    }
  }
  Term* new_term = term_factory ? term_factory->create_term(terms.size(), name)
                                : new Term(terms.size(), name);
  terms.push(new_term);
  return  new_term;
}


void  Grammar::add_start_term(Term* start_term)
{
  Term* rhs_proto[] = { start_term, term(EOF_TERM) };
  Term::array rhs(rhs_proto, 2);
  add_prod(new Prod(term(INI_TERM), rhs));
}


Grammar::Grammar(TermFactory* tf)
{
  term_factory = tf;

  term_by_name("S'");    //INI_TERM
  term_by_name("error"); //ERR_TERM
  term_by_name("$");     //EOF_TERM

  _max_rhs_size = 0;
}


void  Grammar::add_prod(Prod* prod)
{
  prod->id = prods.size();
  prods.push(prod);
  prod->lhs_term->_prods.push(prod);
  _max_rhs_size = max(_max_rhs_size, prod->rhs_terms.size());
}


void  Grammar::prepare()
{
  calc_nullables();
  calc_first_sets();
}
