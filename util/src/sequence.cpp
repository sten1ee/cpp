#include "util/sequence.h"
#define SET sorted_vector
#include "util/setoper.h"
int nseq;

bool  Seq::isEmpty() const
{
  return conj.empty() && disj.empty();
}


bool  Seq::isTrivial() const
{
  return !(conj & disj).empty();
}


bool  Seq::implies(const Seq& seq) const
{
  return is_subset(conj, seq.conj)
      && is_subset(disj, seq.disj);
}


bool  Sequencer::adjoin(Seq* newseq)
{
  if (bContradictory) {
    delete newseq;
    return false;
  }
  if (newseq->isEmpty()) {
    delete newseq;
    seq.flush();
    bContradictory = true;
    return false;
  }
  if (newseq->isTrivial()) {
    delete newseq;
    return true;
  }
  int  cur = seq.size();
  seq.push(newseq);
  AtomSet common;
LOOP:
  while (cur < seq.size()) {
    for (size_t i=cur; 0 < i--; ) {
      if (seq[cur]->implies(*seq[i])) {
        delete seq[i];
        seq.erase(seq.begin() + i);
        cur--;
      }
      else if (seq[i]->implies(*seq[cur])) {
        delete seq[cur];
        seq.erase(seq.begin() + cur);
        goto LOOP;
      }
    }
    for (size_t j=cur; 0 < j--; ) {
      Seq res;
      res.conj = seq[cur]->conj | seq[j]->conj;
      res.disj = seq[cur]->disj | seq[j]->disj;
      common   = res.conj & res.disj;
      if (common.size() == 1) {
        res.conj.erase(res.conj.find(common[0]));
        res.disj.erase(res.disj.find(common[0]));
        if (res.isEmpty()) {
          seq.flush();
          seq.push(new Seq(res));
          bContradictory = true;
          return false;
        }
        seq.push(new Seq(res));
      }
    }
    cur++;
  }
  return true;
}
