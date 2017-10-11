#define __USELOCALES__
#include <ctype.h>
#include "jacc_lexer.h"


string  jacc_lexer::read_C_ident()
{
  ASSERT(isalpha(cc()));
  string id;
  do {
    id.append((char)cc());
    next_char();
  } while (isalnum(cc()) || cc() == '_');
  return id;
}

int  jacc_lexer::next_token(YYSTYPE* tk)
{
RESCAN:
	while (isspace(cc()))
  	next_char();

  // set tok_loc to start position of the new token:
  sync_token_location();

  switch (cc()) {
    case EOF :
      return 0; // 0 designates end of input

    case '%' : {
      next_char();
      if (cc() == '%') {
        next_char();
        return PROP;
      }
      string kw = read_C_ident();
      jacc_property* kw_prop = keywords.get_property(kw);
      if (kw_prop == 0) {
        errorf("Unrecognized %%%s", kw.c_str());
        goto RESCAN;
      }
      return kw_prop->int_val();
    }

    case ':' :
      next_char();
      if (cc() == ':' && nc() == '=') {
        next_char();
        next_char();
      }
      return DERIVES;

    case '/' :
      if (nc() == '/')
        skip_CPP_comment();
      else if (nc() == '*')
        skip_C_comment();
      else
        break;
      goto RESCAN;

    case '{' :
      tk->str = tab_cstr(read_balanced_str('}'));
      return CODE_STMT;

    case '(' :
      tk->str = tab_cstr(read_balanced_str(')'));
      return CODE_EXPR;

    case '<' :
      tk->str = tab_cstr(read_balanced_str('>'));
      return CODE_DECL;

    case ';' : next_char(); return SEMI;
    case '|' : next_char(); return BAR;
    case ',' : next_char(); return COMMA;

    default  :
      if (isalpha(cc())) {
        tk->str = tab_cstr(read_C_ident());
        return ID;
      }

  }
  // prevent error msg from spreading on multiple lines:
  int nc = this->nc();
  if (isspace(nc))
    nc = ' ';
  errorf("Lexical error (no token starts with: `%c%c')", (char)cc(), (char)nc);
  return 0;
}

string  jacc_lexer::read_balanced_str(int clos_c, const char* escape_cs)
{
  string res;
  int open_c = cc();
  int pc; // previous char
  int nopen  = 1;
  while (pc = cc(), next_char()) {
  AFTER_SKIP:
    if (cc() == clos_c && !strchr(escape_cs, pc)) {
      if (--nopen == 0) {
        next_char(); // skip
        return res;
      }
    }
    else if (cc() == open_c && !strchr(escape_cs, pc)) {
      ++nopen;
    }
    else if (cc() == '\"' || cc() == '\'') {
      char qc = (char)cc();
      res.append(qc);
      res.append(read_str(qc));
      res.append(qc);
      goto AFTER_SKIP;
    }
    else if (cc() == '/') {
      if (nc() == '*') {
        skip_C_comment();
        pc = '/';
        goto AFTER_SKIP;
      }
      else if (nc() == '/') {
        skip_CPP_comment();
        pc = '\n';
        goto AFTER_SKIP;
      }
    }
    res.append((char)cc());
  }
  errorf("Unbalanced %cstring%c", (char)open_c, (char)clos_c);
  return res;
}

string  jacc_lexer::read_str(int clos_c, const char* escape_cs)
{
  string res;
  int open_c = cc();
  int pc; // previous char
  while (pc = cc(), next_char()) {
    if (cc() == clos_c && !strchr(escape_cs, pc)) {
      next_char(); // skip
      return res;
    }
    res.append((char)cc());
  }
  errorf("Unterminated %cstring%c", (char)open_c, (char)clos_c);
  return res;
}

void  jacc_lexer::skip_C_comment()
{
  ASSERT(cc() == '/' && nc() == '*');
  next_char();
  while (next_char()) {
    if (cc() == '*' && nc() == '/') {
      next_char();
      next_char();
      return;
    }
  }
  error("Unterminated C-style comment");
}

void  jacc_lexer::skip_CPP_comment()
{
  ASSERT(cc() == '/' && nc() == '/');

  while (next_char()) {
    if (cc() == '\n') {
      next_char();
      break;
    }
  }
}

const char*  jacc_lexer::tab_cstr(const char* cstr)
{
  jacc_property_set::finger f = literals.find(cstr);
  if (!f) {
    literals.insert(f, jacc_property(strdup(cstr)));
  }
  return (*f).key();
}

void  jacc_lexer::set_source(jacc_source* src)
{
  source = src;
}

void  jacc_lexer::push_source(jacc_source* src)
{
  src->master_source = source;
  source = src;
}

jacc_source* jacc_lexer::pop_source()
{
  jacc_source* src = source;
  source = source->master_source;
  return src;
}

void  jacc_lexer::verrorf(const char* fmt, va_list argptr)
{
  ASSERT(source != 0);
  fprintf(log_file, "%s(%d,%d) Error: ", source->name(),
                    _token_location.line, _token_location.col);
  vfprintf(log_file, fmt, argptr);
  fprintf(log_file, "\n");
  if (source->master_source != 0) {
    fprintf(log_file, "  (include trace:");
    for (jacc_source* src  = source->master_source;
                      src != 0; src = src->master_source) {
      fprintf(log_file, " %s(%d,%d);", src->name(),
                        src->location().line, src->location().col);
    }
    fprintf(log_file, ")\n");
  }
  nerrors++;
}

void  jacc_lexer::errorf(const char* fmt, ...)
{
  va_list  argptr;
  va_start(argptr, fmt);
  verrorf(fmt, argptr);
  va_end(argptr);
}

void  jacc_lexer::error(const char* msg)
{
  errorf("%s", msg);
}

void  jacc_lexer::vmessagef(const char* fmt, va_list argptr)
{
  vfprintf(log_file, fmt, argptr);
  fprintf(log_file, "\n");
}

void  jacc_lexer::messagef(const char* fmt, ...)
{
  va_list  argptr;
  va_start(argptr, fmt);
  vmessagef(fmt, argptr);
  va_end(argptr);
}

void  jacc_lexer::message(const char* msg)
{
  messagef("%s", msg);
}

void  jacc_lexer::init()
{
  source   = 0;
  log_file = stderr;
  nerrors  = 0;

  keywords.property("left"       ).set_val(LEFT       );
  keywords.property("right"      ).set_val(RIGHT      );
  keywords.property("nonassoc"   ).set_val(NONASSOC   );
  keywords.property("token"      ).set_val(TOKEN      );
  keywords.property("type"       ).set_val(TYPE       );
  keywords.property("prec"       ).set_val(PREC       );
  keywords.property("when"       ).set_val(WHEN       );
  keywords.property("with"       ).set_val(WITH       );
}

jacc_lexer::~jacc_lexer()
{
  for (jacc_property_set::iterator l  = literals.begin();
                                   l != literals.end();
                                 ++l) {
    free((char*)(*l).key());
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  jacc_source
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
jacc_source::jacc_source(std::istream& src_stream, const string& src_name)
  : _stream(src_stream), _name(src_name)
{
  _location.line = _location.col = 1;
  _cc = _stream.get();
  _nc = _stream.get();
  master_source = 0;
}


bool jacc_source::next_char()
{
  if (_cc == EOF)
    return false;
  if (_cc == '\n')
    _location.line++, _location.col = 1;
  else
    _location.col++;
  _cc = _nc;
  _nc = _stream.get();
  return true;
}
