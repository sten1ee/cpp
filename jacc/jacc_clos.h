// A general command line options parser
#ifndef jacc_clos_h
#define jacc_clos_h

#include <stdio.h>
#include "util/set.h"

struct jacc_clo
{
  enum type_t {
    CLO_FLAG,    // flag option - no parameter
    CLO_PARAM,   // string option with parameter
  };
  type_t        type;
  const char*   name;
  bool          present; // was the option parsed ?

  union {
    bool*       p_flag;
    const char**p_param;
  };

  void  init(const char* n)
    {
      name = n;
      present = false;
    }

  jacc_clo(const jacc_clo& c)
    {
      type = c.type;
      name = c.name;
      present = c.present;
      ASSERT(sizeof(p_flag) == sizeof(p_param));
      ASSERT((void*)&p_flag == (void*)&p_param);
      p_flag = c.p_flag;
    }

  jacc_clo(const char* name, bool* pf)
    : type(CLO_FLAG)
    {
      ASSERT(pf);
      init(name);
      p_flag = pf;
    }

  jacc_clo(const char* name, const char**pp)
    : type(CLO_PARAM)
    {
      ASSERT(pp);
      init(name);
      p_param = pp;
    }

  DEFINE_SET_BY_CSTR_KEY(jacc_clo, name);
};

struct jacc_clos : jacc_clo::set
{
  const char* app_name;
  const char* p_arg;

  jacc_clos(const char* app_name)
    : app_name(app_name), p_arg(0)
    {;}

  // returns the number of errors encountered
  int   parse(int argc, const char* argv[]);
  void  list_options(FILE* out);
};

#endif //jacc_clos_h
