#ifndef jacc_symtype_h
#define jacc_symtype_h

#include "util/set.h"
#include "util/string.h"

#include "jacc_render_opts.h"
#include "jacc_spacer.h"

class  ostream;
struct jacc_log;
struct jacc_sval;
struct jacc_term;

struct jacc_symtype
{
  enum kind_t { THE_SYM, VALUE, CUSTOM };

  kind_t        kind;
  const char*   type_name;  // null if this is an anonymous symtype
  const char*   value_type; // null if this is not a value symtype
  jacc_term*    specz_term; // null if this is not a speczed symtype

  // creates the the $SYM$ symtype; there's a single instance of this kind.
  jacc_symtype();
  // creates a normal (VALUE or CUSTOM) symtype
  jacc_symtype(const char* type_name, const char* value_type);
  // creates a speczed symtype named 'type_name' based on 'base' symtype.
  jacc_symtype(const jacc_symtype& base, jacc_term* specz_term,
               const char* type_name);

  static kind_t  kind_for(const char* type_name, const char* value_type);
  bool   is_identical_with(const jacc_symtype& other) const;

  DEFINE_NAMED_PTR_SET_BY_CSTR_KEY(named_set, jacc_symtype, type_name);
  DEFINE_NAMED_PTR_SET_BY_CSTR_KEY(value_set, jacc_symtype, value_type);

  void  render_stack_decl (std::ostream& out, const jacc_tab& tab,
                                              const jacc_sval& sval, int sidx,
                                              const jacc_render_opts& opt);
  void  render_stack_val  (std::ostream& out, int sidx,
                                              const jacc_render_opts& opt);
  void  render_stack_sym  (std::ostream& out, int sidx,
                                              const jacc_render_opts& opt);
  void  render_result_decl(std::ostream& out, const jacc_tab& tab,
                                              const jacc_render_opts& opt);
  void  render_result_val (std::ostream& out, const jacc_render_opts& opt);
  void  render_result_sym (std::ostream& out, const jacc_render_opts& opt);

  // returns true iff a typedef was rendered:
  bool  render_typedef(std::ostream& out, const jacc_render_opts& opt);
};

struct jacc_symtype_mgr
{
private:
  jacc_log*               log;
  jacc_symtype            the_sym_symtype;
  jacc_symtype::value_set value_symtypes;
  jacc_symtype::named_set named_symtypes;

  jacc_symtype&  value_symtype_for(const char* value_type);
public:
  jacc_symtype&  default_symtype();
  jacc_symtype&  symtype_for(const char* type_name, const char* value_type);
  jacc_symtype&  specz_symtype_for(jacc_symtype& symtype, jacc_term* term,
                                   const char* type_name=0);
  jacc_symtype&  specz_symtype_for(jacc_term* term,
                                   const char* type_name=0);
  void  render_symtype_defs(std::ostream& out, const jacc_render_opts& opt);

  void  set_log(jacc_log* l) { log = l; }
};

inline
jacc_symtype::kind_t  jacc_symtype::kind_for(const char* type_name,
                                             const char* value_type)
{
  return value_type ? VALUE
       : type_name  ? CUSTOM
       :              THE_SYM;
}

#endif //jacc_symtype_h
