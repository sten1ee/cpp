#ifndef jacc_glue_h
#define jacc_glue_h

#ifdef __cplusplus

extern "C" {
#endif

enum tnt_t {
  TNT_UNDEF,
  TNT_TERMINAL,
  TNT_NONTERMINAL
};

enum prec_t {
  PREC_LEFT,
  PREC_RIGHT,
  PREC_NONASSOC
};

void  jg_prop(const char* prop_name, const char* prop_value);
void  jg_type(enum tnt_t, const char* type_name, const char* value_type);
void  jg_type_term(const char* term_name);
void  jg_prec(enum prec_t);
void  jg_prec_term(const char* term_name);
void  jg_prod(const char* lhs_term_name);
void  jg_rhs_part_term(const char* term_name, const char* alias);
void  jg_rhs_part_code(const char* action_code, const char* alias);
void  jg_rhs_part_prec(const char* prec_term_name);
void  jg_rhs_part_when(const char* when_code_expr);
void  jg_rhs_part_with(const char* with_code_expr);
void  jg_rhs(void);

void  jg_error (const char* msg);
void  jg_errorf(const char* fmt, ...);
int   jg_lex(void);

int   yyparse(void);

#ifdef __cplusplus
} //extern "C"
#endif

#endif //jacc_glue_h
