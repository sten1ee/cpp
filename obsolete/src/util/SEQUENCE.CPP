#include "util/sequence.h"
#include "util/setoper.h"


bool  Seq::isEmpty() const
{
  return conj.isEmpty() && disj.isEmpty();
}


bool  Seq::isTrivial() const
{
  return !(conj & disj).isEmpty();
}


bool  Seq::implies(const Seq& seq) const
{
  return isSubset(conj, seq.conj)
      && isSubset(disj, seq.disj);
}


bool  Sequencer::adjoin(Seq* newseq)
{
  if (bContradictory) {
    return false;
  }
  if (newseq->isEmpty()) {
    seq.flush();
    seq.push(newseq);
    bContradictory = true;
    return false;
  }
  if (newseq->isTrivial()) {
    delete newseq;
    return true;
  }
  size_t  cur = seq.size();
  seq.push(newseq);
  AtomSet common;
  Seq     res;
  res.conj.ownsData(false);
  res.disj.ownsData(false);
LOOP:
  while (cur < seq.size()) {
    for (size_t i=cur; 0 < i--; ) {
      if (seq[cur]->implies(*seq[i])) {
        seq.removeAt(i);
        cur--;
      }
      else if (seq[i]->implies(*seq[cur])) {
        seq.removeAt(cur);
        goto LOOP;
      }
    }
    for (size_t j=cur; 0 < j--; ) {
      res.conj = seq[cur]->conj | seq[j]->conj;
      res.disj = seq[cur]->disj | seq[j]->disj;
      common   = res.conj & res.disj;
      if (common.size() == 1) {
        res.conj.remove(common[0]);
        res.disj.remove(common[0]);
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
