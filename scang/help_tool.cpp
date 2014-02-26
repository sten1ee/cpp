#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <iostream.h>
#include "exe/scanner.h"

/*
void _main(int nn, char** aa)
{
  char buf[512];
  int i=1, inc=0;
  while (0 < --nn) {
    if (0 == strcmp(aa[i], "i")) {
      inc = 1;
      break;
    }
  }
  while (gets(buf)) {
    if (buf[0] == '~') {
      puts("~");
      return;
    }
    sprintf(buf + strlen(buf), " %i", i);
    puts(buf);
    i += inc;
  }
}
*/

class istream_buffer : public scan_buffer {
  istream& in;
  public:
    istream_buffer(istream& is)
      : in(is)
      {
      }

    int read(char_t* pstore, int maxstore) const {
      if (in.getline(pstore, maxstore))
        return in.gcount();
      else
        return EOF;
    }
};

void main(/*int nn, char** aa*/)
{
  istream_buffer buf(cin);
  scanner scan(&buf);

  int tok;
  do {
    tok = scan.next_token();
    cout << "next_token (" << tok << ") is `";
    for (int i=0; i < scan.token_len(); ++i) {
      int c = scan.token_beg()[i];
      if (isprint(c))
        cout << (char)c;
      else
        cout << "ascii(" << c << ")";
    }
    cout << "'\n";
  } while (0 <= tok);
}

