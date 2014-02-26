#include "jacc_symtype.h"
#include "jacc.h"


// creates the $SYM$ symtype; normally there's a single instance of this kind.
jacc_symtype::jacc_symtype()
  : kind(THE_SYM),
    type_name(0), value_type(0),
    specz_term(0)
{
}

// creates a normal (VALUE or CUSTOM) symtype
jacc_symtype::jacc_symtype(const char* tn, const char* vt)
  : kind(kind_for(tn, vt)),
    type_name(tn), value_type(vt),
    specz_term(0)
{
}

// creates a speczed symtype based on 'base' symtype
jacc_symtype::jacc_symtype(const jacc_symtype& base, jacc_term* st,
                           const char* tn)
  : kind(base.kind),
    type_name(tn),
    value_type(base.value_type),
    specz_term(st)
{
  ASSERT(kind != CUSTOM);
}

jacc_symtype&
jacc_symtype_mgr::value_symtype_for(const char* value_type)
{
  ASSERT(value_type != 0);
  jacc_symtype::value_set::finger f = value_symtypes.find(value_type);
  if (!f) {
    value_symtypes.insert(f, new jacc_symtype(0, value_type));
  }
  return *(*f);
}

jacc_symtype&
jacc_symtype_mgr::symtype_for(const char* type_name, const char* value_type)
{
  if (type_name == 0) { // anonymous symtype
    if (value_type == 0)
      return default_symtype();
    else
      return value_symtype_for(value_type);
  }

  // the tougher case (named symtype) :
  jacc_symtype::named_set::finger f = named_symtypes.find(type_name);
  if (!f) { // luckily this is the first occurance of this type name
    named_symtypes.insert(f, new jacc_symtype(type_name, value_type));
  }
  else if (!(*f)->is_identical_with(jacc_symtype(type_name, value_type))) {
    log->errorf("Redefinition of type `%s' is not identical", type_name);
  }
  return *(*f);
}

jacc_symtype&
jacc_symtype_mgr::specz_symtype_for(jacc_term* term,
                                    const char* type_name)
{
  return specz_symtype_for(default_symtype(), term, type_name);
}

jacc_symtype&
jacc_symtype_mgr::specz_symtype_for(jacc_symtype& symtype, jacc_term* term,
                                    const char* type_name)
{
  // specialization only works for anonymously typed terminals so:
  if (symtype.type_name != 0 || term->tnt == TNT_NONTERMINAL)
    return symtype;

  if (type_name == 0)
    type_name = term->name.c_str();

  jacc_symtype::named_set::finger f = named_symtypes.find(type_name);
  if (f) { // oops ! typename clash
    log->errorf("Redefinition of type `%s' is not identical", type_name);
  }
  else {
    named_symtypes.insert(f, new jacc_symtype(symtype, term, type_name));
  }
  return *(*f);
}

jacc_symtype&
jacc_symtype_mgr::default_symtype()
{
  return the_sym_symtype;
}

bool
jacc_symtype::is_identical_with(const jacc_symtype& other) const
{
  return kind       == other.kind
      && type_name  == other.type_name
      && value_type == other.value_type
      && specz_term == other.specz_term;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  code rendering methods
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
jacc_symtype::render_stack_decl( std::ostream&          out,
                                const jacc_tab&         tab,
                                const jacc_sval&        sval,
                                      int               sidx,
                                const jacc_render_opts& opt)
{
  out << tab << opt.sym << "* " << sval.alias << opt.sym_suffix << " = ";
  render_stack_sym(out, sidx, opt);
  out << ';' << tab;

  if (kind == VALUE)
    out << value_type << "& " << sval.alias << " = static_cast<"
        << opt.value_sym << "< " << value_type << " >*>(" << sval.alias
        << opt.sym_suffix << ')' << opt.value << ';';
  else if (kind == CUSTOM)
    out << type_name << "* " << sval.alias << " = " << opt.custom_type_downcast
        << '<' << type_name << "*>(" << sval.alias << opt.sym_suffix << ");";
  // else (kind == THE_SYM) - no need to declare value
}

void
jacc_symtype::render_stack_val(std::ostream& out, int sidx,
                                             const jacc_render_opts& opt)
{
  if (kind == THE_SYM)
    out << opt.sym_value;
  else if (kind == VALUE) {
    out << "static_cast<" << opt.value_sym<< "< " << value_type << " >*>(";
    render_stack_sym(out, sidx, opt);
    out << ')' << opt.value;
  }
  else {//(kind == CUSTOM)
    out << opt.custom_type_downcast << '<' << type_name << "*>(";
    render_stack_sym(out, sidx, opt);
    out << ')';
  }
}

void
jacc_symtype::render_stack_sym(std::ostream& out, int sidx,
                                             const jacc_render_opts& opt)
{
  out << "static_cast<" << opt.sym << "*>("
      << opt.stack_top << '[' << sidx << "])";
}

void
jacc_symtype::render_result_decl(std::ostream& out, const jacc_tab& tab,
                                                    const jacc_render_opts& opt)
{
  out << tab;
  if (kind == THE_SYM)
    // no need to declare 'result'
    out << opt.sym << "* " << opt.result << opt.sym_suffix
        << " = new " << opt.sym << ';';
  else if (kind == VALUE)
    out << opt.value_sym << "< " << value_type << " >* "
        << opt.result << opt.sym_suffix << " = new "
        << opt.value_sym << "< " << value_type << " >;"
        << tab << value_type << "& " << opt.result << " = "
        << opt.result << opt.sym_suffix << opt.value << ';';
  else //  kind == CUSTOM
    // no need to declare 'result_sym'
    out << type_name << "* " << opt.result << opt.custom_result_init
        << ' ' << type_name << ';';

  // $CUSTOM_RESULT_INIT$ should be set to "= new" or "; //"
}

void
jacc_symtype::render_result_val(std::ostream& out, const jacc_render_opts& opt)
{
  if (kind == THE_SYM)
    out << opt.sym_value;
  else if (kind == VALUE)
    out << opt.result;
  else  // kind == CUSTOM
    out << '*' << opt.result;
}

void
jacc_symtype::render_result_sym(std::ostream& out, const jacc_render_opts& opt)
{
  if (kind == CUSTOM)
    out << opt.custom_type_upcast << '(' << opt.result << ");";
  else // kind == THE_SYM || kind == VALUE
    out << opt.result << opt.sym_suffix;
}

bool
jacc_symtype::render_typedef(std::ostream& out, const jacc_render_opts& opt)
{
  if (specz_term != 0) {
    if (kind == THE_SYM)
      out << "typedef " << opt.specz_sym << '<';
    else
      out << "typedef " << opt.specz_value_sym << '<' << value_type << ", ";
    out << specz_term->id << ">  " << opt.tag_specz
        << type_name << opt.tag_end_specz << ';';
    return true;
  }
  if (kind == VALUE && type_name != 0) {
    out << "typedef " << opt.value_sym << "< "
        << value_type << " >  " << type_name << ';';
    return true;
  }

  return false; // to indicate no typedef was rendered
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  a routine to render the symtype(s) declarations
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void
jacc_symtype_mgr::render_symtype_defs(std::ostream& out, const jacc_render_opts& opt)
{
  for (jacc_symtype::named_set::iterator inst  = named_symtypes.begin();
                                         inst != named_symtypes.end();
                                       ++inst)
    if ((*inst)->render_typedef(out, opt))
      out << '\n';
}
