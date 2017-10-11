#ifndef jacc_lexer_h
#define jacc_lexer_h

#include <stdio.h>
#include <stdarg.h>

#include "util/string.h"
#include "util/set.h"
#include "jacc_tab.h"
#include "jacc_props.h"

// class for logging errors, messages, etc.
struct jacc_log
{
  virtual void  verrorf(const char* fmt, va_list argptr)=0;
  virtual void  errorf(const char* fmt, ...)=0;
  virtual void  error (const char* msg)=0;

  virtual void  vmessagef(const char* fmt, va_list argptr)=0;
  virtual void  messagef(const char* fmt, ...)=0;
  virtual void  message (const char* msg)=0;

  virtual int   get_nerrors() const=0;
  virtual void  set_nerrors(int ne)=0;

  virtual void  set_log_file(FILE* f)=0;
};

struct jacc_source
{
  struct location_t // line-column specificator
  {
    int line, col;
  };

  jacc_source(std::istream& src_stream, const string& src_name);

  // reads one more character from the input; returns false on EOF:
  bool  next_char();

  int   cc() const { return _cc; }
  int   nc() const { return _nc; }

  location_t  location() const { return _location; }
  const char* name()     const { return _name.c_str(); }

  private:
    int           _cc, _nc;   // current & next chars of the input stream (IS)
    std::istream& _stream;    // source IS; only next_token() reads it !
    const string  _name;      // IS's name; used to report error locations.
    location_t    _location;  // current position in the IS; used to report
                              // exact error locations.
  public:
    jacc_source*  master_source; // the source that included this source;
                                 // used to keep track of the 'include trace';
                                 // may be NULL;
};

class jacc_lexer : private jacc_log
{
  protected:
    jacc_source* source;

    bool  next_char() { return source->next_char(); }

    int cc() const { return source->cc(); }
    int nc() const { return source->nc(); }

    jacc_source::location_t _token_location;
    void  sync_token_location() { _token_location = location(); }

    // string table (nobody else should care about deleteing lexed strings):
    jacc_property_set  literals;
    // returns an equivalent free-of-need-to-deallocate cstr,
    // that will last till the lexer object is destroyed:
    const char*   tab_cstr(const char* cstr);
    const char*   tab_cstr(const string& str) { return tab_cstr(str.c_str()); }

    // a mapping from keywords to token ids:
    jacc_property_set  keywords;

    // instance initialization method:
    void          init();

    void          skip_C_comment();
    void          skip_CPP_comment();

    string        read_C_ident();

    /** Eats enough of the input stream in order to balance the sequence of
        open_c(s) & clos_c(s).
        While clos_c is explicit parameter, open_c is value of cc in the
        moment of the call.
        The string escape_cs contains the set of so called 'escape chars'.
        The leading open_c and trailing clos_c (although skipped)
        are not part of the result string.
        Sample:
        If input stream contents are: "{ if (c == '{') {return 0;} }???"
        (i.e. cc == '{', nc == ' ')
        the call read_balanced_str('}') will return:
        " if (c == '{') {return 0;} " and the input left will be: "???".
    */
    string        read_balanced_str(int clos_c, const char* escape_cs="\\");
    /** Reads from input till clos_c (or EOF) is reached.
        The string escape_cs contains the set of so called 'escape chars'.
        The leading open_c and trailing clos_c (although skipped)
        are not part of the result string.
        It is intended to read C-style string & character literals.
    */
    string        read_str(int clos_c, const char* escape_cs="\\");

    // implement the jacc_log interface:
    FILE* log_file;  // error & message output stream (defaults to stderr)
    int   nerrors;   // number of errors logged
    void  set_log_file(FILE* f) { log_file = f; }
    void  verrorf(const char* fmt, va_list argptr);
    void  errorf(const char* fmt, ...);
    void  error (const char* msg);
    void  vmessagef(const char* fmt, va_list argptr);    
    void  messagef(const char* fmt, ...);
    void  message (const char* msg);
    int   get_nerrors() const { return nerrors; }
    void  set_nerrors(int ne) { nerrors = ne; }

  public:
    void          set_source(jacc_source* source);
    void          push_source(jacc_source* source);
    jacc_source*  pop_source();

    int    next_token(YYSTYPE* tok);

    jacc_source::location_t  token_location  () const
      {
        return _token_location;
      }
    jacc_source::location_t  location() const
      {
        return source->location();
      }

    jacc_log*   get_log() { return this; }

    jacc_lexer(jacc_source* source) { init(); set_source(source); }
    jacc_lexer() { init(); }

    virtual ~jacc_lexer();
};

#endif //jacc_lexer_h
