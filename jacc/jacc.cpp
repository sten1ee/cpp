#include <ctype.h>
#include <fstream>
#include <iomanip>
#ifdef TIME_JACC
# include <sys/timeb.h>
#endif

#include "jacc.h"
#include "jacc_spacer.h"
#include "jacc_clos.h"
#include "lalr_print.h"
#include "util/msg_listener.h"
#ifdef ALLOCATOR_DIAG
# include "util/diag.h"
#endif

bool  jacc::code_refers_to_ident(const char* code, const char* ident)
{
  const char* occ;
  if (code == 0)
    return false;
  for (occ = strstr(code, ident); occ != 0; occ = strstr(occ + 1, ident)) {
    if (code < occ && isalnum(occ[-1]))
      continue;
    if (occ + strlen(ident) < code + strlen(code)
      && isalnum(occ[strlen(ident)]))
      continue;
    return true;
  }
  return false;
}

int  jacc::count_terminal_actions(const LALR_action::set& actions)
{
  int res = 0;
  for (LALR_action::set::const_iterator iact  = actions.begin();
                                        iact != actions.end();
                                      ++iact)
    if ((*iact)->la_term->is_terminal())
      res++;
  return res;
}

int  jacc::count_nonterminal_actions(const LALR_action::set& actions)
{
  return actions.size() - count_terminal_actions(actions);
}

void  jacc::render_action_table(std::ostream& out)
{
  // Shift and goto S is encoded as   S+1
  // Reduce by rule R is encoded as -(R+1)
  // Accept is encoded as -1 (i.e. Reduce by rule 0)
  // Error is encoded as 0

  bool compress = this->compress_table;
  int* reduce_counts = (compress ? new int[grammar.n_prods()] : 0);
  for (int i = 0; i < table.n_states(); i++)
    {
      const LALR_state* state = table.state(i);
      int def_reduce =
            (compress ? table.calc_default_reduce(state, reduce_counts) : -1);
      int def_count  = (0 <= def_reduce ? reduce_counts[def_reduce] : 0);
      out << "\n  static short s" << i << "[] =" << "\n  {" << std::setw(3)
          << 2*(count_terminal_actions(state->actions) - def_count + 1) << ",";
      int  nprint = 0;
      for (LALR_action::set::const_iterator iact  = state->actions.begin();
                                            iact != state->actions.end();
                                          ++iact)
        {
          const LALR_action& act = *(*iact);
          if (!act.la_term->is_terminal())
            continue;
          if (act.getf(LALR_action::DO_REDUCE)
           && act.reduce_prod->id == def_reduce)
            continue;

          if (nprint++ % 7 == 0)
            out << "\n   ";
          out << std::setw(3) << act.la_term->id << ',' << std::setw(4);
          if (act.getf(LALR_action::DO_SHIFT))
            out <<  (act.shift_state->id + 1);
          else if (act.getf(LALR_action::DO_REDUCE))
            out << -(act.reduce_prod->id + 1);
          else if (act.getf(LALR_action::DO_ERROR))
            out <<  0;
          else if (act.getf(LALR_action::DO_ACCEPT))
            out << -1;
          else
            ASSERT(0);
          out << ',';
        }
      // Default entry
      // Note that if def_reduce == -1 (i.e. no default reduce)
      // -(def_reduce + 1) == -(-1 + 1) == 0 which encodes Error
      //
      out << "\n    -1," << std::setw(4) << -(def_reduce + 1)
          << "\n  };";
    }
  if (compress)
    delete[] reduce_counts;

  out << "\n  static short *action_tab[] ="
      << "\n  {";
  out.setf(std::ios::left);
  for (int j=0; j < table.n_states(); j++)
    {
      if (j % 8 == 0)
        out << "\n    ";
      out << 's' << std::setw(3) << j << "+1";
      if (j != table.n_states()-1)
        out << ", ";
    }
  out.unsetf(std::ios::left);
  out << "\n  };"
      << "\n  return action_tab;";
}

void  jacc::render_reduce_table(std::ostream& out)
{
  // Goto S  is encoded as  S
  // ERROR   is encoded as -1
  // DEFAULT is encoded as -1

  for (int i = 0; i < table.n_states(); i++)
    {
      const LALR_state& s = *table.state(i);
      int   nt_acts = count_nonterminal_actions(s.actions);
      if (nt_acts == 0)
        continue;

      out << "\n  static short s" << i << "[] ="
          << "\n  { " << std::setw(3) << 2*(nt_acts + 1) << ",";
      int nprint = 0;
      for (LALR_action::set::const_iterator iact  = s.actions.begin();
                                            iact != s.actions.end();
                                          ++iact)
        {
          const LALR_action& act = *(*iact);
          if (act.la_term->is_terminal())
            continue;

          ASSERT(act.getf(LALR_action::DO_SHIFT));
          if (nprint++ % 8 == 0)
            out << "\n    ";
          out << std::setw(3) << act.la_term->id << ","
              << std::setw(3) << act.shift_state->id << ",";
        }
      out << "\n     -1, -1"
          << "\n  };";
    }

  out << "\n  static short *reduce_tab[] ="
      << "\n  {";
  out.setf(std::ios::left);
  for (int j=0; j < table.n_states(); j++)
    {
      if (j % 8 == 0)
        out << "\n    ";
      if (0 == count_nonterminal_actions(table.state(j)->actions))
        out << "     0";
      else
        out << 's' << std::setw(3) << j << "+1";

      if (j != table.n_states()-1)
        out << ", ";
    }
  out.unsetf(std::ios::left);
  out << "\n  };"
      << "\n  return reduce_tab;";
}

void  jacc::render_production_table(std::ostream& out)
{
  out << "\n  static prod_entry  production_tab[] ="
      << "\n  {";

  for (int i = 0; i < grammar.n_prods(); i++)
    {
      Prod* prod = grammar.prod(i);
      if (i % 8 == 0)
        out << "\n    ";
      out << '{' << std::setw(3) << prod->lhs_term->id << ','
                 << std::setw(2) << prod->rhs_terms.size() << '}';
      if (i != grammar.n_prods()-1)
        out << ',';
    }

  out << "\n  };"
      << "\n  return production_tab;";
}

void  jacc::render_delete_table(std::ostream& out)
{
  out << "\n  static del_entry  delete_tab[] ="
      << "\n  {"
      << "\n      0, ";
  for (int i=1; i < grammar.n_prods(); ++i) {
    jacc_prod* prod = SURE_CAST(grammar.prod(i), jacc_prod);
    unsigned long del_word = 0;
    for (int j=0; j < prod->rhs_terms.size(); ++j) {
      del_word <<= 1;
      jacc_term* term = SURE_CAST(prod->rhs_terms[j], jacc_term);
      if (term->symtype == 0
       || term->symtype->kind != jacc_symtype::CUSTOM)
        del_word |= 1;
    }
    if (i % 14 == 0)
      out << "\n    ";
    out << std::setw(3) << del_word;
    if (i != 0)
      out << ", ";
  }
  out << "\n  };"
      << "\n  return delete_tab;";
}

void  jacc::render_symtype_enum(std::ostream& out)
{
  const char* tag_symtype     = do_get_macro_body("<SYMTYPE>");
  const char* tag_end_symtype = do_get_macro_body("</SYMTYPE>");
  const char* eof_id          =    get_macro_body("EOF_ID", "eof");

  // indicates discontinuity in the enum range, so the next const
  // must be given an explicit value:
  bool dc = true;
  // indicates there is previous const defined so we must put comma */
  bool hp = false;
  for (int i=0; i < grammar.n_terms(); ++i) {
    if (i == jacc_grammar::INI_TERM || i == jacc_grammar::ERR_TERM
      || !grammar.term(i)->is_terminal())
      {
        dc = true;
        continue;
      }
    if (hp)
      out << ",";
    else
      hp = true;
    out << "\n  " << tag_symtype
        << (i == jacc_grammar::EOF_TERM ? eof_id
                                        : grammar.term(i)->name.c_str())
        << tag_end_symtype;
    if (dc)
      {
        out << " = " << i;
        dc = false;
      }
  }
}

void  jacc::render_symtype_defs(std::ostream& out)
{
  jacc_render_opts opt;
  DEBUGIN(zfill(&opt));
  opt.value_sym       = do_get_macro_body("VALUE_SYM");
  opt.specz_sym       = do_get_macro_body("SPECZ_SYM");
  opt.specz_value_sym = do_get_macro_body("SPECZ_VALUE_SYM");
  opt.tag_specz       = do_get_macro_body("<SPECZ>");
  opt.tag_end_specz   = do_get_macro_body("</SPECZ>");

  // Attach a symtype to the eof_term:
  jacc_term* eof_term = grammar.term(jacc_grammar::EOF_TERM);
  ASSERT(eof_term->symtype == 0);
  const char* eof_id = get_macro_body("EOF_ID", "eof");
  eof_term->symtype = &symtype_mgr.specz_symtype_for(eof_term, eof_id);

  // Transfer the call to who can handle it:
  symtype_mgr.render_symtype_defs(out, opt);
}

void  jacc_prod::render(std::ostream& out) const
{
  out << lhs_term->name;
  if (is_mra_prod())
    out << " (midrule action)";
  else
    {
      out << " ::=";
      for (int i=0; i < sval_env.top_idx; ++i)
        {
          out << ' ' << rhs_terms[i]->name;
          if (sval_env[i].alias != 0)
            out << '(' << sval_env[i].alias << ')';
        }
    }
}

void  jacc::render_do_action(std::ostream& out)
{
  jacc_render_opts opt;
  DEBUGIN(zfill(&opt));
  opt.sym                  = do_get_macro_body("SYM");
  opt.value_sym            = do_get_macro_body("VALUE_SYM");
  opt.sym_suffix           = do_get_macro_body("SYM_SUFFIX");
  opt.value                = do_get_macro_body("VALUE");
  opt.sym_value            = do_get_macro_body("SYM_VALUE");
  opt.stack_top            = do_get_macro_body("STACK_TOP");
  opt.result               = do_get_macro_body("RESULT");
  opt.custom_result_init   = do_get_macro_body("CUSTOM_RESULT_INIT");
  opt.custom_type_downcast = do_get_macro_body("CUSTOM_TYPE_DOWNCAST");
  opt.custom_type_upcast   = do_get_macro_body("CUSTOM_TYPE_UPCAST");

  jacc_tab tab(1);
  for (int i = 0; i < grammar.n_prods(); i++)
    {
      out << tab   << "case " << i << ": // ";
      if (i == 0) // this is the start prod S' -> S.$
        {         // note that it is not a jacc_prod instance !
          out << "accept"
              << tab++ << '{';
          out << tab << "RESULT = (0);"
              << tab << "break;";
          out << --tab << '}';
          continue;
        }
      jacc_prod*    prod = SURE_CAST(grammar.prod(i), jacc_prod);
      jacc_symtype* result_symtype =
                           SURE_CAST(prod->lhs_term, jacc_term)->symtype;
      const char*   action_code = prod->action_code;
      // print some human readable production designator in the comment
      // after the case (15 lines above):
      prod->render(out);
      out << tab++ << '{';
      if (action_code != 0)
        {
          // declare the val variables:
          int top_idx = prod->sval_env.top_idx;
          for (int j = 0; j < top_idx; ++j)
            {
              const jacc_sval& sval = prod->sval_env[j];
              if (sval.alias != 0)
                {
                  sval.symtype->render_stack_decl(out, tab, sval,
                                                  sval.idx - top_idx, opt);
                }
            }
        }
      result_symtype->render_result_decl(out, tab, opt);
      out << '\n'
          << tab << (action_code ? action_code : "// (no user action)") << ';'
          << '\n'
          << tab << "RESULT = ";
          result_symtype->render_result_sym(out, opt);
      out << ';'
          << tab << "break;";
      out << --tab << '}';
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generated code is based on a jacc template file
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool
jacc::try_handle_directive(const string& directive_name)
{
  void (jacc::*renderer)(std::ostream& os) = 0;

  if (jacc_jamp::try_handle_directive(directive_name))
    return true;

  else if (directive_name == "render_action_table")
    renderer = &jacc::render_action_table;

  else if (directive_name == "render_reduce_table")
    renderer = &jacc::render_reduce_table;

  else if (directive_name == "render_production_table")
    renderer = &jacc::render_production_table;

  else if (directive_name == "render_delete_table")
    renderer = &jacc::render_delete_table;

  else if (directive_name == "render_do_action")
    renderer = &jacc::render_do_action;

  else if (directive_name == "render_symtype_enum")
    renderer = &jacc::render_symtype_enum;

  else if (directive_name == "render_symtype_defs")
    renderer = &jacc::render_symtype_defs;

  else return false;

  if (pos != 0)
    (this->*renderer)(*pos);

  // The initial idea was that the rendered text should be preprocessed,
  // but this proved to be extremely inefficient for large grammars (eg java12):
#if 0
  ostrstream os;
  (this->*renderer)(os);
  int   sz  = os.pcount();
  char* buf = os.str();
  if (buf != 0)
    {
      jacc_jamp::process(istrstream(buf, sz), directive_name);
      delete[] buf;
    }
#endif
  return true;
}

static  // returns the file name only - no path, no extension(s)
string  get_grm_name(const char* grm_file_name)
{
  const char* cc = grm_file_name;
  // skip the 'path' part of the file name:
  size_t  len;
  while ((len = strcspn(cc, "\\/")) != strlen(cc))
    cc += (len + 1);

  if (cc == 0)
    cc = grm_file_name;
  string grm_name;
  if (isalpha(*cc) || *cc == '_')
    do
      grm_name.append(*cc++);
    while (isalnum(*cc) || *cc == '_');

  return grm_name;
}

#if 0
// Gets the file name only (no path, no extension)
static
string  get_file_name(const char* file_name)
{
  const char* pend = strrchr(base_file_name, '.');
  if (pend == 0)
    pend = file_name + strlen(file_name);

  const char* pbeg1 = strrchr(pend, '\\');
  const char* pbeg2 = strrchr(pend, '/');
  if (pbeg1 == 0)
    if (pbeg2 == 0)
      pbeg = file_name;
    else
      pbeg = pbeg2;
  else
    if (pbeg2 == 0)
      pbeg = pbeg1;
    else
      pbeg = (pbeg1 < pbeg2 ? pbeg2 : pbeg1);

  return string(pbeg, pend - pbeg);
}
#endif

// Given a base file name and an extension constructs a new file name:
// (preserving the path and the file name part)
// make_file_name("c:\utils\jacc.exe", ".tmpl") -> "c:\utils\jacc.tmpl"
// make_file_name("d:\prj\calc.j", ".out")      -> "d:\prj\calc.out"
// make_file_name("/bin/jacc", ".xxx")          -> "/bin/jacc.xxx"
// make_file_name("/usr/stj/calc.j", ".yyy")    -> "/usr/stj/calc.yyy"
static
string  make_file_name(const char* base_file_name, const char* file_ext)
{
  const char* pend = strrchr(base_file_name, '.');
  if (pend == 0)
    pend = base_file_name + strlen(base_file_name);
  else if (strchr(pend, '\\') != 0 || strchr(pend, '/') != 0)
    pend += strlen(pend);

  string res_file_name;
  res_file_name.append(base_file_name, pend - base_file_name)
               .append(file_ext);
  return res_file_name;
}

enum { // enumerate codes returned from the process method
  P_SUCCESS,
  PERR_PARSE_CLO,
  PERR_OPEN_GRM_FILE,
  PERR_PROCESS_GRM_FILE,
  PERR_OPEN_TMPL_FILE,
  PERR_PROCESS_TMPL_FILE,
  PWRN_CREATE_OUT_FILE = 0x100, // possibly or-ed with some actual err code
};

int  jacc::process(int argc, const char* argv[])
{
  const char* app_name = "jacc";
  const char* grm_file_name;
  const char* tmpl_file_name      = 0;
  bool        create_out_file     = false;
  bool        show_progress       = false;
  bool        print_noncores      = false;
  bool        print_resolved_cnfls= false;
  bool        print_lookaheads    = false;
  bool        no_render_code      = false;
  bool        no_resolve_cnfls    = false;
  bool        no_compress_table   = false;

  {  // --- Local block ---
    jacc_clos clos(app_name);
    bool help = false;
    bool info = false;
    clos.insert(jacc_clo("h", &help));
    clos.insert(jacc_clo("i", &info));
    clos.insert(jacc_clo("t", &tmpl_file_name));
    clos.insert(jacc_clo("v", &create_out_file));
    clos.insert(jacc_clo("-show-progress",            &show_progress));
    clos.insert(jacc_clo("-print-noncore-items",      &print_noncores));
    clos.insert(jacc_clo("-print-lookahead-syms",     &print_lookaheads));
    clos.insert(jacc_clo("-print-resolved-conflicts", &print_resolved_cnfls));
    clos.insert(jacc_clo("-no-render-code",           &no_render_code));
    clos.insert(jacc_clo("-no-resolve-conflicts",     &no_resolve_cnfls));
    clos.insert(jacc_clo("-no-compress-table",        &no_compress_table));
    int nerrs = clos.parse(argc, argv);
    if (info)
      {
        fprintf(stdout, "%s",
"... Jacc is Just Another Compiler Compiler or at least it pretends to be ...\n"
"It is authored by Stanislav Jordanov (stenlee@gmail.com)\n"
"                           Dedication: For Joanna"//        Sofia, build date:" __DATE__
        );
        return P_SUCCESS;
      }
    if (clos.p_arg != 0)
    	{
      	grm_file_name = clos.p_arg;
      }
    else if (!help)
      { // missing grm file name when option -h is not specified is an error:
        ++nerrs;
        fprintf(stderr, "%s: No grammar file given\n", app_name);
      }
    if (0 < nerrs || help)
      {
        fprintf(stdout, "Usage: %s [options] grammar-file\n", app_name);
        fprintf(stdout, "Available options:\n");
        const char* opt_fmt = "\t%s\n";
        fprintf(stdout, opt_fmt, "-h     \t print this help & exit");
        fprintf(stdout, opt_fmt, "-i     \t print program info & exit");
        fprintf(stdout, opt_fmt, "-t <fn>\t use template file <fn>");
        fprintf(stdout, opt_fmt, "-v     \t verbose mode - create .out file");
        fprintf(stdout, opt_fmt, "--show-progress");
        fprintf(stdout, opt_fmt, "--print-noncore-items");
        fprintf(stdout, opt_fmt, "--print-lookahead-syms");
        fprintf(stdout, opt_fmt, "--print-resolved-conflicts");
        fprintf(stdout, opt_fmt, "--no-render-code");
        fprintf(stdout, opt_fmt, "--no-resolve-conflicts");
        fprintf(stdout, opt_fmt, "--no-compress-table");
        return nerrs ? PERR_PARSE_CLO : P_SUCCESS;
      }
    // initialize the instance flag used by render_action_table:
    this->compress_table = !no_compress_table;
  }  // --- /Local block ---

  std::ifstream  grm_file(grm_file_name);

  if (!grm_file)
    {
      fprintf(stderr, "%s: Unable to open grammar file `%s'",
                      app_name, grm_file_name);
      return PERR_OPEN_GRM_FILE;
    }

  { // --- Local block ---
    // (Pre)define some crucial macros.
    // If user (accidently) tries to redefine any of them
    // (using a property declaration in the grammar file)
    // this will result in an error message.
    //
    char  buf[8];
    macros.property("START_STATE").set_val("0");
    macros.property("START_PROD") .set_val("0");
    sprintf(buf, "%d", jacc_grammar::EOF_TERM);
    macros.property("EOF_SYM")    .set_val(tab_cstr(buf));
    sprintf(buf, "%d", jacc_grammar::ERR_TERM);
    macros.property("ERROR_SYM")  .set_val(tab_cstr(buf));
  }  // --- /Local block ---

  parse_grammar(grm_file, grm_file_name);
  grm_file.close();
  if (get_nerrors())
    {
      fprintf(stderr, "%s: %d error(s) processing grammar file `%s'",
                      app_name, get_nerrors(), grm_file_name);
      return PERR_PROCESS_GRM_FILE;
    }

  grammar.prepare();
  fprintf_msg_listener  progress_listener(stdout);
  if (show_progress)
    table.msg_listener = &progress_listener;
  table.prepare_from(grammar);
  if (0 != table.n_conflicts())
    {
      if (no_resolve_cnfls)
        fprintf(stderr, "%s: Grammar contains %d (potentially resolvable) "
                        "conflict(s)\n", app_name, table.n_conflicts());
      else
        {
          int n_unresolved = table.resolve_conflicts();
          if (n_unresolved != 0)
            fprintf(stderr, "%s: Grammar contains %d unresolved conflict(s)\n",
                            app_name, n_unresolved);
        }
    }

  if (create_out_file)
    {
      string  out_file_name = make_file_name(grm_file_name, ".out");
      std::ofstream  out_file(out_file_name.c_str());
      if (out_file)
        {
          int pf = PF_DEFAULTS;
          if (print_noncores)       pf |= PF_PRINT_STATE_NON_CORE_ITEMS;
          if (print_lookaheads)     pf |= PF_PRINT_ITEM_LA_TERMS;
          if (print_resolved_cnfls) pf |= PF_PRINT_RESOLVED_CONFLICTS;
          if (compress_table)       pf |= PF_COMPRESS_TABLE;
          print_setf(pf);
          print_grammar(out_file, grammar);
          print_LALR_table(out_file, table);
        }
      else // not fatal enough to abort execution, just report:
        fprintf(stderr, "%s: Unable to create file `%s'\n",
                        app_name, out_file_name.c_str());

    }

  if (no_render_code) // we were asked just to process the grammar
    return P_SUCCESS;

  // At this point we should have a clear grammar name.
  // The user might have defined it in the grammar file using property
  // definition. Otherwise the grammar name is derived from
  // the grammar file name. In this case get_grm_name() is used - it
  // skips the path and then scans the longes valid C identifier (or "")
  //
  if (!macros.defined("GRAMMAR_NAME"))
    {
      string grm_name = get_grm_name(grm_file_name);
      if (grm_name != "")
        macros.property("GRAMMAR_NAME").set_val(tab_cstr(grm_name.c_str()));
    }
  // Another required definition:
  if (!macros.defined("DELETE_WORD_TYPE"))
    {
      const char* word_type = "unsigned long long";
      if (grammar.max_rhs_size() <= 16)
        word_type = "unsigned short";
      else if (grammar.max_rhs_size() <= 32)
        word_type = "unsigned long";
      else
        error("max_rhs_size > 32 ");
      macros.property("DELETE_WORD_TYPE").set_val(word_type);
    }
  // Definitions regarding parse result type:
  if (!macros.defined("PARSE_RESULT_TYPE"))
    {
      jacc_symtype* res_symtype = grammar.get_start_term()->symtype;
      switch (res_symtype->kind)
        {
        case jacc_symtype::CUSTOM:
          macros.property("PARSE_RESULT_TYPE_IS_CUSTOM").set_val("true");
          macros.property("PARSE_RESULT_TYPE").set_val(res_symtype->type_name);
          break;
        case jacc_symtype::VALUE:
          macros.property("PARSE_RESULT_TYPE_IS_VALUE").set_val("true");
          macros.property("PARSE_RESULT_TYPE").set_val(res_symtype->value_type);
          break;
        case jacc_symtype::THE_SYM:
          macros.property("PARSE_RESULT_TYPE_IS_SYM").set_val("true");
          // no need to define PARSE_RESULT_TYPE
          break;
        default:
          ASSERT(0);
        }
    }

  // Time to open the template file
  if (tmpl_file_name == 0)
    { // template file name was not supplied as a command line option
      // so use defaults:
      tmpl_file_name = tab_cstr(make_file_name(argv[0], ".tmpl"));
    }

  std::ifstream  tmpl_file(tmpl_file_name);
  if (!tmpl_file)
    {
      fprintf(stderr, "%s: Unable to open template file `%s'",
                      app_name, tmpl_file_name);
      return PERR_OPEN_TMPL_FILE;
    }

  jacc_jamp::process(tmpl_file, tmpl_file_name);
  if (get_nerrors())
    {
      fprintf(stderr, "%s: %d error(s) processing template file `%s'",
                      app_name, get_nerrors(), tmpl_file_name);
      return PERR_PROCESS_TMPL_FILE;
    }

  return P_SUCCESS;
}

#ifdef TIME_JACC
double  operator - (const struct timeb& t1, const struct timeb& t2)
{
  long mls = (t1.time - t2.time) * 1000 + (t1.millitm - t2.millitm);
  return (mls / 1000) + (mls % 1000) / 1000.0;
}
#endif

int  main(int argc, char* argv[])
{
#ifdef TIME_JACC
  timeb  tb1, tb2;
  ftime(&tb1);
#endif
  int res = jacc().process(argc, (const char**)argv);
#ifdef TIME_JACC
  ftime(&tb2);
  cout << "Finished in " << (tb2 - tb1) << " sec";
#endif
#ifdef ALLOCATOR_DIAG
# ifdef TIME_JACC
  printf("\n");
# endif
  diag::print_allocator_stats(stdout);
#endif
  return res;
}
