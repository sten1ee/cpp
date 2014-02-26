#include "chain.h"

void main(void)
{
  chain<int> c;
  c.insert_front(5);
  c.insert_front(3);
  c.insert_front(1);
  chain<int> d;
  d.insert_front(4);
  d.insert_front(2);
  d.insert_front(0);
  c |= d;  
}
