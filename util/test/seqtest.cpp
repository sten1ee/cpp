#include "util/sequence.h"
#include <iostream.h>


ostream& operator << (ostream& os, const Seq& seq)
{
  int i;
  os << '{';
  for (i = 0; i < seq.conj.size(); i++) {
    if (i) {
      os << ", ";
    }
    os << char('a' + seq.conj[i]);
  }
  os << " -> ";
  for (i = 0; i < seq.disj.size(); i++) {
    if (i) {
      os << ", ";
    }
    os << char('a' + seq.disj[i]);
  }
  return os << '}';
}


ostream& operator << (ostream& os, const Sequencer& ss)
{
  for (int i=0; i < ss.seq.size(); i++) {
    os << *ss.seq[i] << endl;
  }
  return os;
}


void  main()
{
  atom_t  a=0, b=1, c=2;

  Seq* s1 = new Seq();
       s1->conj.insert(a);
       s1->disj.insert(b);
  Seq* s2 = new Seq();
       s2->conj.insert(b);
       s2->disj.insert(c);
  Seq* s3 = new Seq();
       s3->disj.insert(a);
  Seq* s4 = new Seq();
       s4->conj.insert(c);

  Sequencer  ss;
  ss.adjoin(s1);
  cout << ss << "----------------" << endl;
  ss.adjoin(s2);
  cout << ss << "----------------" << endl;
  ss.adjoin(s3);
  cout << ss << "----------------" << endl;
  ss.adjoin(s4);
  cout << ss << "----------------" << endl;
}
