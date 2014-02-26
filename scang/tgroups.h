#ifndef tgroups_h
#define tgroups_h

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

class ostream;

class TGroup {
  public:

    typedef ::vector< vector_traits_ptr< TGroup > > vector;

    struct Id {
      int src;
      int dst;

      Id(int s, int d) : src(s), dst(d) {;}

      typedef ::vector< vector_traits< Id > > vector;
    };

  private:
    const char* _text;
    CharSet     _mask;
    CharSet     _body;
    Id::vector  _ids;

    friend class TGroupMgr;

    static bool  compatible(const TGroup& p, const TGroup& q);

  public:
    const char*       text() const { return _text; }
    const CharSet&    mask() const { return _mask; }
    const CharSet&    body() const { return _body; }
    const Id::vector& ids () const { return _ids;  }

    void  set_text(const char* text);

   ~TGroup()
      {
        delete[] _text;
      }

    TGroup(Id id)
      : _text(0), _ids(1, 0)
      {
        _ids.push(id);
      }

    /*------------------------------------------------------------
    // Combination of groups p and q
    TGroup(TGroup& p, TGroup& q)
      : _mask(p.mask() & q.mask()),
        _body(p.body() | q.body()),
        _ids(p.ids().size() + q.ids().size(), 0)
      {
        _ids.insert(_ids.end(), p.ids().begin(), p.ids().end());
        _ids.insert(_ids.end(), q.ids().begin(), q.ids().end());
      }
    ------------------------------------------------------------*/
    void  merge(TGroup& grp)
      {
        _mask &= grp.mask();
        _body |= grp.body();
        _ids.insert(_ids.end(), grp.ids().begin(), grp.ids().end());
      }

    bool  is_compatible_with(const TGroup& grp) const
      {
        return compatible(*this, grp);
      }
};


class TGroupMgr {
  private:
    // A collection of pairwise incompatible groups:
    TGroup::vector _groups;

    struct TGMapping {
      const TGroup::Id  id;
      const TGroup*     group;

      TGMapping(const TGroup::Id& id, const TGroup* group)
        : id(id), group(group)
        {;}

        struct traits : sortvec_traits_ptr< TGMapping > {
          typedef const TGroup::Id&  DataKey;
          static DataKey  keyof(DataIn tgm) { return tgm->id; }
          static int      compare(DataKey p, DataKey q)
            {
              if (p.src != q.src)
                return p.src - q.src;
              else
                return p.dst - q.dst;
            }
        };

        typedef sorted_vector< traits > set;
    };

    TGMapping::set  dictionary;

    void  remap(const TGroup::Id::vector& ids, TGroup* target);
    void  merge(TGroup* target, TGroup* src);

  public:
    TGroupMgr()
      {;}

    void  adjoin(TGroup* grp); // TGroupMgr takes ownership
    void  adjoin(const State* state, int min_body_size);
    void  adjoin(const FA& fa, int min_body_size);

    const TGroup::vector&  groups() const { return _groups; }
    const TGroup*          lookup_group(int src, int dst) const
            {
              TGMapping::set::finger f =
                  const_cast<TGMapping::set&>(dictionary)
                      .find(TGroup::Id(src, dst));
              return f ? (*f)->group : 0;
            }
};

#endif
