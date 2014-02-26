#ifndef jacc_jamp_h
#define jacc_jamp_h

#include "jacc_lexer.h"

struct jacc_jamp : jacc_lexer
{
  public:  enum token_t
  {
    TT_EOF,
    TT_PHRASE,
    TT_DIRECTIVE
  };

  public:  typedef jacc_property jacc_macro;

  public:    jacc_property_set  macros;
  protected: ostream*           pos;
  protected: bool               del_pos;// 'del_pos' - are we supposed to delete
                                        // pos when we are getting rid of it ?
  protected: bool               expand_macros;
  public:  void  set_expand_macros(bool em) { expand_macros = em; }

  private: void  init()
    {
      pos     = 0;
      del_pos = false;
      expand_macros = true;
    }

  private: bool  set_output_file(const char* file_name);

  public: jacc_jamp()
    {
      init();
    }

  public: ~jacc_jamp()
    {
      if (del_pos)
        delete pos;
    }

  protected: void  output(const string& str)
    {
      if (pos)
        *pos << str.c_str();
    }

  protected: void  output(const char* cstr)
    {
      if (pos)
        *pos << cstr;
    }

  // Reads & returns phrase, directive name or EOF:
  protected: token_t   next_token(string& buf);

  // Reads up to '$' or eol or eof (removes '$' from input stream);
  // If eol or eof is reached - returns false, otherwise returns true;
  private:   bool      read_till_dlr(string& buf);

  // Calls read_till_dlr(...) and displays the appropriate error msg
  // if eol or eof was reached before '$'
  protected: bool      read_macro_name(string& macro_name);

  // Calls read_till_dlr(...) and displays the appropriate error msg
  // if eol or eof was reached before '$'
  protected: bool      read_directive_name(string& directive_name);

  // If the macro is defined - returns its body,
  // else prints an error message and returns a mock body -
  // the macro name enclosed in '?' symbols.
  protected: const char* do_get_macro_body(const char* macro_name);
  // If the macro is defined - returns its body,
  // else returns NULL.
  protected: const char* try_get_macro_body(const char* macro_name);
  // If the macro is defined - returns its body,
  // else defines it as 'default_body' and acts like if it was defined
  // (default_body should be a stable string as it is not tab_ctr-ed)
  protected: const char* get_macro_body(const char* macro_name,
                                        const char* default_body);

  // Handles the directive (if implementing class is capable of handling it)
  // and returns true, or else does nothing and returns false.
  // This class handles "default" and "file" directives.
  protected: virtual bool  try_handle_directive(const string& directive_name);

  // Calls try_handle_directive(...) amd if it fails to handle,
  // (i.e. returns false) prints an error message.
  protected: void  handle_directive(const string& directive_name);

  // Process the specified input stream
  public:    void  process(istream& src_stream, const string& src_name);
};


#endif //jacc_jamp_h
