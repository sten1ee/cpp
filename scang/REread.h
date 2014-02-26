#ifndef PG_REREAD_H
#define PG_REREAD_H

#include "re.h"
#include "util/sortvec.h"
#include <iostream.h>

class REContext
{

    void  ErrorReadRE(const char* errmsg);
    RE*   Read_macro(istream& is);
    RE*   Read_charset(istream& is);
    RE*   Read_term(istream& is);
    RE*   Read_iterats(istream& is);
    RE*   Read_concats(istream& is);
    RE*   Read_alterns(istream& is);
    RE*   GetMacro(const char* name);

    struct Macro
      {
        const char* name;
        const RE*   body;

        Macro(const char* n, const RE* b)
          {
            name = strcpy(new char[strlen(n) + 1], n);
            body = b;
          }

       ~Macro()
          {
            delete[] name;
            delete body;
          }

        struct sv_traits : sortvec_traits_ptr< Macro >
          {
            typedef const char* DataKey;
            static DataKey  keyof(DataIn m) { return m->name; }
            static int      compare(DataKey k1, DataKey k2) { return strcmp(k1, k2); }
          };

        typedef sorted_vector< sv_traits > set;
      };

    Macro::set  macros;

  public:

    REContext();

    RE*   ReadRE(istream& is);
    void  DefineMacro(const char* name, const char* body);
};


#endif//PG_REREAD_H
