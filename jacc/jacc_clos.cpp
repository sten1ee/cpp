#include "jacc_clos.h"

int  jacc_clos::parse(int /*argc*/, const char* argv[])
{
  int nerr = 0;
  int i    = 1; // initialize to 1 in order to skip app name
  while (argv[i] != 0) {
    const char* arg = argv[i++];
    if (*arg == '-') {
      finger f = find(arg + 1);
      if (!f) {
        fprintf(stderr, "%s: Invalid command line option `%s'\n",
                        app_name, arg);
        nerr++;
        continue;
      }
      jacc_clo& clo = (*f);
      if (clo.present) {
        fprintf(stderr, "%s: Duplicate command line option `%s'\n",
                        app_name, arg);
        nerr++;
        continue;
      }
      else
        clo.present = true;

      if (clo.type == jacc_clo::CLO_PARAM) {
        if (argv[i] == 0) {
          fprintf(stderr, "%s: Command line option `%s' requires parameter\n",
                          app_name, arg);
          nerr++;
          break;
        }
        else
          *clo.p_param = argv[i++];
      }
      else {
        ASSERT(clo.type == jacc_clo::CLO_FLAG);
        *clo.p_flag = true;
      }
    }
    else { // this is the argument rather than a command line option
      if (p_arg == 0) {
        p_arg = arg;
      }
      else {
        fprintf(stderr, "%s: Duplicate command line argument `%s'\n",
                        app_name, arg);
        nerr++;
      }
    }
  }

  return nerr;
}

void  jacc_clos::list_options(FILE* out)
{
  for (iterator opt = begin(); opt != end(); ++opt)
    fprintf(out, "\t-%s %s\n", (*opt).name,
                 ((*opt).type == jacc_clo::CLO_PARAM ? "<...>" : ""));
}
