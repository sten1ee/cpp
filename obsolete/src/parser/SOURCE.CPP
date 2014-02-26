#include <iostream.h>
#include "parser/source.h"
#include "parser/debug.h"


Source::Source(istream& is)
  : is(is),
    lineno(0),
    line(128, DOUBLE_INCREMENT)
{
}


const char* Source::nextLine()
{
  if (is.peek() == EOF) {
    return 0;
  }
  ///////////////////////
  //if (is.peek() == '$') {
  //  is.get();
  //  return 0;
  //}
  //cout << lineno << ": " << line.c_str() << endl;
  ///////////////////////
  line.flush();
  lineno++;
  while (true) {
    int c = is.get();
    switch (c) {
      case '\n':
      case EOF :
        line.push('\0');
        return &line[0];
      default:
        line.push((char)c);
    }
  }
}


LinCol  Source::pos(const char* cp)
{
  PARSER_ASSERT(&line[0] <= cp && cp <= &line[0] + line.size() - 1);
  return LinCol((unsigned short)lineno,
                (unsigned short)((cp - &line[0]) + 1));
}
