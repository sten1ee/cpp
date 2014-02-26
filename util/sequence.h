#ifndef UTIL_SEQUENCE_H
#define UTIL_SEQUENCE_H
#include <iostream.h>
#ifndef   UTIL_SORTVEC_H
#include "util/sortvec.h"
#endif

/***********************************************************
  the Atom type - (user should customize)
***********************************************************/
typedef long atom_t;

struct AtomTraits : sortvec_traits< atom_t >
{
  static int  compare(atom_t a1, atom_t a2)
    { return a1 - a2; }
};

typedef sorted_vector< AtomTraits > AtomSet;

/***********************************************************
  class Seq
  A sequence in logic is an ordered pair of atom sets: <conj, disj>
  Let conj = {p1, p2, ... pn} and disj = {q1, q2, ... qm}
  The sequence <conj, disj> is usually denoted as:
    {p1, p2, ... pn} -> {q1, q2, ... qm} and is interpreted as the
    following statement: 'if every pi is true then some qj is true'
    or in logics: 'p1 & p2 & ... pn -> q1 | q2 | ... qm'
  Seqs with empty conj:
    {} -> {q1, q2, ... qm} is interpreted: 'some qj is true'
  Seqs with empty disj:
    {p1, p2, ... pn} -> {} is interpreted: 'some pi is false'
  Unconditionally true  atom: {   -> a }
  Unconditionally false atom: { a ->   }
  Contradiction (or the empty) sequence: { -> }
  Trivial seqs: if conj and disj have (at least one) common
    element the sequence is senseless (exercise: why?)
  Implication: we say 'seq1 imlplies seq2'  if
    seq1.conj is a subset of seq2.conj &
    seq1.disj is a subset of seq2.disj
    In this case seq1 is more general statement then seq2
    or written in logics: 'seq1 -> seq2'
***********************************************************/
extern int nseq;
class Seq
{
  public:
    AtomSet   conj;
    AtomSet   disj;

    Seq(Seq& s) : conj(s.conj), disj(s.disj) {}
    Seq() {}

    bool  isEmpty()   const; //iff conj & disj are both the empty set;
    bool  isTrivial() const; //iff conj & disj have common element;
    bool  implies(const Seq& seq) const;
};

/***********************************************************
  class Sequencer - the modus ponens deductive engine
***********************************************************/
class Sequencer
{
  public:
    vector< vector_traits_ptr< Seq > > seq;
    bool  bContradictory; //true <-> the empty seq has been adjoined or deduced
                          //no more adjoin(s) can take part.
    Sequencer() : bContradictory(false) {}

    bool  adjoin(Seq* newseq); //caller relinquishes ownership
};

/***********************************************************
  Modus ponens ... and nothing more or less ...
  Sample usage:
  ...
  atom_t  a=0, b=1, c=2;

  Seq* s1 = new Seq();     //s1: {a -> b}
       s1->conj.insert(a);
       s1->disj.insert(b);
  Seq* s2 = new Seq();     //s2: {b -> c}
       s2->conj.insert(b);
       s2->disj.insert(c);
  Seq* s3 = new Seq();     //s3: {  -> a}
       s3->disj.insert(a);
  Seq* s4 = new Seq();     //s4: {c ->  }
       s4->conj.insert(c);

  Sequencer  sqr;
  sqr.adjoin(s1); //sqr: {{a -> b}}
  sqr.adjoin(s2); //sqr: {{a -> b}, {b -> c}, {a -> c}}
  sqr.adjoin(s3); //sqr: {{  -> a}, {  -> c}, {  -> b}}
  sqr.adjoin(s4); //sqr: {{ -> }} and adjoin returns false !;
  //now sqr.bContradictory == true
  //further adjoins will have no effect !
  ...
***********************************************************/

#endif //UTIL_SEQUENCE_H
