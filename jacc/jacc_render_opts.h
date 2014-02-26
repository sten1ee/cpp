#ifndef jacc_render_opts_h
#define jacc_render_opts_h

#include <string.h> // for memset

struct jacc_render_opts
{
  const char* sym;       // The actual name of the sym type
  const char* value_sym; // The actual name of the 'value_sym' template type
  const char* sym_suffix;// Suffix that is added to the alias of a val
                         // to form the alias of its sym
  const char* value;     // Suffix that is added to a value sym to get its value
  const char* sym_value; // The (print) value of a sym object
  const char* stack_top; // Expression that reffers to the stack top
  const char* result;    // The actual name of the `result'
  const char* custom_result_init;   // '= new' || '; //'
  const char* custom_type_downcast; // defaults to 'static_cast'
  const char* custom_type_upcast;   // defaults to ''

  const char* specz_sym;       // The actual name of the specz_sym template type
  const char* specz_value_sym; // The actual name of the specz_value _sym tmpl..
  const char* tag_specz;       // Prefix attached to specz_... instance typedefs
  const char* tag_end_specz;   // Suffix attached to specz_... instance typedefs
};

inline void zfill(jacc_render_opts* opt)
{
  memset(opt, 0, sizeof(jacc_render_opts));
}

#endif // jacc_render_opts_h
