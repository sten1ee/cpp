#include "tgroups.h"
#include "fa.h"


bool  TGroup::compatible(const TGroup& s1, const TGroup& s2)
{
  CharSet::const_iterator  b1 = s1.body().begin(), end_b1 = s1.body().end();
  CharSet::const_iterator  b2 = s2.body().begin(), end_b2 = s2.body().end();

  CharSet::const_iterator  m1 = s1.mask().begin(), end_m1 = s1.mask().end();
  CharSet::const_iterator  m2 = s2.mask().begin(), end_m2 = s2.mask().end();

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


void  TGroup::set_text(const char* n)
{
  _text = strcpy(new char[strlen(n) + 1], n);
}


void  TGroupMgr::remap(const TGroup::Id::vector& ids, TGroup* target)
{
  for (TGroup::Id::vector::const_iterator  ii = ids.begin(), ie = ids.end();
                                           ii != ie;
                                         ++ii) {
    const TGroup::Id& id = *ii;
    TGMapping::set::finger f = dictionary.find(id);
    if (f) {
      (*f)->group = target;
    }
    else {
      dictionary.insert(f, new TGMapping(id, target));
    }
  }
}


void  TGroupMgr::merge(TGroup* target, TGroup* src)
{
  remap(src->ids(), target);
  target->merge(*src);
  delete src;
}


void  TGroupMgr::adjoin(TGroup* grp)
{
  typedef TGroup::vector::iterator Iterator;
  DEBUGIN(TGroup* match = 0;)
  for (Iterator gi  = _groups.begin(), ge = _groups.end();
                gi != ge;
              ++gi) {
    TGroup* g = *gi;
    if (g->is_compatible_with(*grp)) {
      ASSERT(match == 0);
      DEBUGIN(match = g;)
      DEBUGIN(continue;)
      merge(g, grp);
      return;
    }
  }
  DEBUGIN(if (match) { merge(match, grp); return; })
  remap(grp->ids(), grp);
  _groups.push(grp);
}


struct group_traits : sortvec_traits_ptr<TGroup>
{
  typedef int  DataKey;
  static  DataKey   keyof(DataIn data) { return data->ids()[0].dst; }
  // Note: use revers order: q - p (not p - q)
  static  int       compare(DataKey p, DataKey q) { return q - p;}
};


void  TGroupMgr::adjoin(const State* state, int min_body_size)
{
  typedef sorted_vector<group_traits> GroupSet;
  GroupSet groups(OWNS_DATA);
  const State::Transitions& transitions = state->transitions();
  for (State::TransitionsIter ti  = transitions.begin(), te = transitions.end();
                              ti != te;
                            ++ti) {
    State* to_state = (*ti).state;
    GroupSet::finger f = groups.find(to_state->id());
    if (!f) {
      groups.insert(f, new TGroup(TGroup::Id(state->id(), to_state->id())));
    }
    (*f)->_body.insert_back((*ti).charr);
  }

  CharSet state_mask;
  for (int i=groups.size(); 0 <= --i; ) {
    TGroup* group = groups[i];
    if (group->body().size() < min_body_size) {
      state_mask |= group->body();
    }
  }

  for (int i=groups.size(); 0 <= --i; ) {
    TGroup* group = groups[i];
    if (min_body_size <= group->body().size()) {
      group->_mask = state_mask;
      state_mask |= group->body();
      adjoin(group);
      groups.erase(groups.begin() + i);
    }
  }
}


void  TGroupMgr::adjoin(const FA& fa, int min_body_size)
{
  const FA::States& states = fa.states();
  for (FA::StatesIter si = states.begin(); si != states.end(); ++si) {
    adjoin(*si, min_body_size);
  }
}


/*
int  main()
{
  TGroupMgr tgm;

  CharSet m1, b1;
  m1.insert('a');
  b1.insert('b');
  b1.insert('c');
  tgm.adjoin(new TGroup(m1, b1, TGroup::Id(0, 1)));

  CharSet m2, b2;
  m2.insert('b');
  b2.insert('a');
  b2.insert('c');
  m2.insert('d');
  tgm.adjoin(new TGroup(m2, b2, TGroup::Id(1, 2)));
}
*/
