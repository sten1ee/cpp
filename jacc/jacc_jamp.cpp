#include "jacc_jamp.h"
#include <fstream>

bool  jacc_jamp::set_output_file(const char* file_name)
{
  if (del_pos)
    delete pos;

  std::ostream* new_pos = new std::ofstream(file_name);
  if (!*new_pos) {
    errorf("Unable to open output file `%s'", file_name);
    delete new_pos;
    pos = 0;
    del_pos = false;
    return false;
  }
  else {
    pos = new_pos;
    del_pos = true;
    return true;
  }
}

const char*
jacc_jamp::do_get_macro_body(const char* macro_name)
{
  jacc_macro* macro = macros.get_property(macro_name);
  if (macro != 0) { // ok - macro is defined
    return macro->str_val();
  }
  else { // oops ! somebody is in a real trouble:
    errorf("Undefined macro `%s'", macro_name);
    string  res;
    res.append('?').append(macro_name).append('?');
    return tab_cstr(res);
  }
}

const char*
jacc_jamp::try_get_macro_body(const char* macro_name)
{
  jacc_property_set::finger f = macros.find(macro_name);
  if (!f) {
    return 0;
  }
  return (*f).str_val();
}

const char*
jacc_jamp::get_macro_body(const char* macro_name, const char* default_body)
{
  jacc_property_set::finger f = macros.find(macro_name);
  if (!f) {
    macros.insert(f, jacc_macro(macro_name, default_body));
  }
  return (*f).str_val();
}

jacc_jamp::token_t
jacc_jamp::next_token(string& token)
{
  token = "";
  sync_token_location();

  if (cc() == '$' && nc() == '!') {
    next_char();
    next_char();
    if (cc() == '$') { // this was the $!$ delimiter
      next_char();   // skip & continue
    }
    else {
      read_directive_name(token);
      return TT_DIRECTIVE;
    }
  }

  if (cc() == EOF)
    return TT_EOF;

  while (true) {
    if (cc() == EOF) {
      return TT_PHRASE;
    }
    if (cc() == '$') {
      if (nc() == '!') {  // we've reached a DIRECTIVE;
        return TT_PHRASE; // return the accumulated phrase;
      }
      next_char();
      if (cc() == '/' && nc() == '/') { // template comment (skip)
        skip_CPP_comment();
        continue;
      }
      if (cc() == '$') { // "$$" stands for single "$"
        next_char();
        token.append('$');
        continue;
      }
      string macro_name;
      read_macro_name(macro_name);
      if (expand_macros) { // to handle properly ifdef / ifndef
        token.append(do_get_macro_body(macro_name.c_str()));
      }
      else {
        token.append('$').append(macro_name).append('$');
      }
    }
    else { // normal char
      token.append((char)cc());
      next_char();
    }
  } // while
}

bool  jacc_jamp::read_macro_name(string& macro_name)
{
  if (!read_till_dlr(macro_name)) {
    error("Unterminated macro specification");
    return false;
  }
  else
    return true;
}

bool  jacc_jamp::read_directive_name(string& directive_name)
{
  if (!read_till_dlr(directive_name)) {
    error("Unterminated directive specification");
    return false;
  }
  else
    return true;
}

bool  jacc_jamp::read_till_dlr(string& buf)
{
  sync_token_location();
  while (true) {
    switch (cc()) {
    case EOF:
    case '\n':
      return false;
    case '$':
      next_char();
      return true;
    default:
      buf.append((char)cc());
      next_char();
    }
  }
}

bool  jacc_jamp::try_handle_directive(const string& directive_name)
{
  if (directive_name == "default") {
    // Syntax:
    // $!default$<macro-name-phrase><macro-body-phrase>
    // Semantics:
    // if macro-name is not defined, defines it as macro-body.
    string macro_name;
    if (TT_PHRASE != next_token(macro_name)) {
      error("default directive missing macro name");
      return true;
    }
    string macro_body;
    if (TT_PHRASE != next_token(macro_body)) {
      error("default directive missing macro body");
      return true;
    }

    jacc_property_set::finger f = macros.find(macro_name.c_str());
    if (!f) // do not forget to use tab_cstr !
      macros.insert(f, jacc_macro(tab_cstr(macro_name),
                                   tab_cstr(macro_body)));
    return true;
  } // default directive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (directive_name == "define") {
    // Syntax:
    // $!define$<macro-name-phrase><macro-body-phrase>
    // Semantics:
    // defines macro-name as macro-body.
    string macro_name;
    if (TT_PHRASE != next_token(macro_name)) {
      error("define directive missing macro name");
      return true;
    }
    string macro_body;
    if (TT_PHRASE != next_token(macro_body)) {
      error("define directive missing macro body");
      return true;
    }

    jacc_property_set::finger f = macros.find(macro_name.c_str());
    if (f)
      {
        errorf("Macro $%s$ redefined as `%s' (was previously defined as: '%s')",
               macro_name.c_str(), macro_body.c_str(), (*f).str_val());
        (*f).set_val(tab_cstr(macro_body));
      }
    else // do not forget to use tab_cstr !
      macros.insert(f, jacc_macro(tab_cstr(macro_name),
                                  tab_cstr(macro_body)));
    return true; //handled
  } // define directive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ( directive_name == "ifdef"
    || directive_name == "ifndef" ) {
    // Syntax:
    // $!if(n)def$<macro-name-phrase><conditional-phrase>
    // Semantics:
    // <conditional-phrase> is output iff macro-name is (un)defined
    string macro_name;
    if (TT_PHRASE != next_token(macro_name)) {
      errorf("%s directive missing macro name", directive_name.c_str());
      return true;
    }

    bool cond_ok = macros.defined(macro_name) == (directive_name == "ifdef");
    if (!cond_ok)
      // disable macro expansion, as the conditional phrase
      // may contain undefined macros:
      set_expand_macros(false);

    string phrase;
    if (TT_PHRASE != next_token(phrase)) {
      errorf("%s directive missing conditional phrase", directive_name.c_str());
      return true;
    }

    if (cond_ok)
      output(phrase);
    else
      set_expand_macros(true);

    return true; //handled
  } // ifdef/ifndef directive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (directive_name == "file") {
    //$!file$<file-name-phrase>
    string file_name;
    if (TT_PHRASE != next_token(file_name)) {
      error("file directive missing file name");
      pos = &std::cerr;
    }
    else
      set_output_file(tab_cstr(file_name));

    return true; //handled
  } // file directive
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  else
    return false; // directive not handled
}

void  jacc_jamp::handle_directive(const string& directive_name)
{
  if (!try_handle_directive(directive_name))
    errorf("Unknown directive `%s'", directive_name.c_str());
}


void  jacc_jamp::process(std::istream& src_stream, const string& src_name)
{
  jacc_source  src(src_stream, src_name);
  push_source(&src);
  string  token;
  while (true) {
    switch (next_token(token)) {
    case TT_EOF:
      pop_source();
      return;
    case TT_PHRASE:
      output(token);
      break;
    case TT_DIRECTIVE:
      handle_directive(token);
      break;
    default:
      ASSERT(0);
    }
  }
}

#ifdef JAMP_MAIN
#ifdef __WIN32__
# define PATH_SEPARATOR '\\'
#else
# define PATH_SEPARATOR '/'
#endif
int main(int /*argc*/, char* argv[])
{
  if (argc < 2) {
    const char* prog_name = strrstr(argv[0], PATH_SEPARATOR);
    prog_name = (prog_name ? argv[0]);
    fprintf(stderr, "Usage: %s file-name", prog_name);
    return -1;
  }
  const char*     src_name = argv[1];
  std::ifstream   src_stream(argv[1], ios::nocreate);

  jacc_jamp     jamp;

  jamp.get_log()->set_log_file(stdout);
  jamp.process(src_stream, src_name);
  return get_log()->get_nerrors();
}
#endif
