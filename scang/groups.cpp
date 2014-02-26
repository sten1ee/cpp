/*
   A group is a pair of disjoint sets called `mask' and `body'.
   Shortly we will denote a group {A} by ~A/A where
   ~A is the `mask' and A is the `body'.

   Let {A}, {B} and {C} are 3 groups.

   Def1: We say that {A} is covered by {B} (or {B} covers {A})
         when A is a subset of ~B union B.
         We denote it by {A} <~ {B}.


   Def2: We say that {A} and {B} are compatible  when {A} <~ {B} and {B} <~ {A}
         We denote it by {A} ~~ {B}.

   Def3: Let {A} ~~ {B}. We define {A} * {B} (A-combined-B) to be the group
         with mask (~A intersection ~B)
         and body (A union B).
         Note that {A} * {B} = {B} * {A} (i.e. `*' is symmetrical).

   Th1:  If {A} ~~ {B} then
         (2) {A} * {B} <~ {C} <-> {A} <~ {C} & {B} <~ {C}

         Proof: Trivial.

   Th2:  If {A} ~~ {B} then
         (2) {C} <~ {A} * {B} <-> {C} <~ {A} & {C} <~ {B}

         Proof: Not as trivial as the proof of Th1.

   Th3:  If {A} ~~ {B} then
         (3) {A} * {B} ~~ {C} <-> {A} ~~ {C} & {B} ~~ {C}

         Proof: A trivial consequence of Th1 & Th2.

   Th4:  If {A} ~~ {B} and {B} ~~ {C} then
         {A} * {B} ~~ {C} <-> {A} ~~ {B} * {C} <--> {A} ~~ {C}

         Proof: A trivial consequence of Th3.

   Th5:  ({A} * {B}) * {C} is equivalent to {A} * ({B} * {C}).
         (Equivalent means that left is defined <-> right is defined
          and if defined both resolve to the same value)
*/

#include "predefs.h"

class TransGroup {
  public:

    typedef ::vector< vector_traits_ptr< TransGroup > > vector;

    struct Id {
      int src;
      int dst;

      Id(int s, int d) : src(s), dst(d) {;}

      typedef ::vector< vector_traits< Id > > vector;
    };

  private:
    CharSet     mask;
    CharSet     body;
    Id::vector  ids;

  public:
    TransGroup(CharSet& m, CharSet& b, Id id)
      : mask(m), body(b), ids(1)
      {
        ids.push(id);
      }

    // Combination of groups p and q
    TransGroup(TransGroup& p, TransGroup& q)
      : mask(p.mask & q.mask),
        body(p.body | q.body),
        ids(p.ids.size() + q.ids.size(), 0)
      {
        ids.insert(ids.end(), p.ids.begin(), p.ids.end());
        ids.insert(ids.end(), q.ids.begin(), q.ids.end());
      }

    void  merge(TransGroup& grp)
      {
        mask &= grp.mask;
        body |= grp.body;
        ids.insert(ids.end(), grp.ids.begin(), grp.ids.end());
      }

    bool  is_compatible_with(const TransGroup& grp) const
      {
        return compatible(*this, grp);
      }

    static bool  compatible(const TransGroup& p, const TransGroup& q);
};


bool  TransGroup::compatible(const TransGroup& s1, const TransGroup& s2)
{
  CharSet::const_iterator  b1 = s1.body.begin(), end_b1 = s1.body.end();
  CharSet::const_iterator  b2 = s2.body.begin(), end_b2 = s2.body.end();

  CharSet::const_iterator  m1 = s1.mask.begin(), end_m1 = s1.mask.end();
  CharSet::const_iterator  m2 = s2.mask.begin(), end_m2 = s2.mask.end();

  while (true) {
    int diff;

    if (b1 == end_b1) {
      if (b2 == end_b2)
        return true;
      else
        diff = 1;
    }
    else {
      if (b2 == end_b2)
        diff = -1;
      else
        diff = CharSet::Traits::compare(*b1, *b2);
    }

    if (diff < 0) {
      do {
        if (m2 == end_m2) {
          return false;
        }
        diff = CharSet::Traits::compare(*b1, *m2);
        ++m2;
      } while (diff < 0);
      if (diff > 0) {
        return false;
      }
      ++b1;
    }
    else if (diff > 0) {
      do {
        if (m1 == end_m1) {
          return false;
        }
        diff = CharSet::Traits::compare(*b2, *m1);
        ++m1;
      } while (diff < 0);
      if (diff > 0) {
        return false;
      }
      ++b2;
    }
    else {
      ++b1;
      ++b2;
    }
  }
}

class GroupMgr {
  public:
    // A collection of pairwise incompatible groups:
    int n_adjoins;

    GroupMgr()
      : n_adjoins(0)
      {;}

    TransGroup::vector _groups;

    void  adjoin(TransGroup* grp); // GroupMgr takes ownership

    const TransGroup::vector&  groups() const { return _groups; }
};

void  GroupMgr::adjoin(TransGroup* grp)
{
  typedef TransGroup::vector::iterator Iterator;
  DEBUGIN(TransGroup* match = 0;)
  n_adjoins++;
  for (Iterator gi  = _groups.begin(), ge = _groups.end();
                gi != ge;
              ++gi) {
    TransGroup* g = *gi;
    if (g->is_compatible_with(*grp)) {
      ASSERT(match == 0);
      DEBUGIN(match = g;)
      g->merge(*grp);
      DEBUGIN(continue;)
      delete grp;
      return;
    }
  }
  DEBUGIN(if (match) { match->merge(*grp); delete grp; return; })
  _groups.push(grp);
}

int  main()
{
  CharSet m1, b1;
  m1.insert('a');
  b1.insert('b');
  b1.insert('c');
  TransGroup g1(m1, b1, TransGroup::Id(0, 1));

  CharSet m2, b2;
  m2.insert('b');
  b2.insert('a');
  b2.insert('c');
  m2.insert('d');
  TransGroup g2(m2, b2, TransGroup::Id(1, 2));

  if (TransGroup::compatible(g1, g2)) {
    TransGroup g(g1, g2);
    return (int)&g;
  }
}
