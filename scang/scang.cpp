#include "scan_buffer.h"

class scanner_base
{
  private:
    scan_buffer*  buffer;

  public:
    typedef int  token_t;

    token_t   next_token();
};

scanner_base::token_t  scanner_base::next_token()
{
  buffer->flush_marked();
  int s = 0;
  while (1) {
    int c = buffer->get();
    switch (s) {
    case 0:;
    }
  }
}
