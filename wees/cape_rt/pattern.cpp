#include "pattern.h"
#include "execenv.h"


#define CHECK_PRECONDITIONS  assert(beg_pos < aset.end_pos() || beg_pos == aset.npos())

class EmptyPattern : public Pattern {
public:
  bool  match(ExecEnv&    /*env*/,
              ASet&       /*aset*/,
              position_t  /*beg_pos*/,
        const Pattern&    /*gama*/) const
  {
    assert(0);
    return true;
  }

#ifdef DEBUG_PATTERNS
  EmptyPattern() { _dbg_string = "<e>"; }
#endif
};

const Pattern&          empty_pattern    = EmptyPattern();
      Annotation       _empty_annotation(atype_t(0));
      Annotation* const empty_annotation = &_empty_annotation;

inline
bool  is_empty_pattern(const Pattern& pattern)
{
  return &pattern == &empty_pattern;
}

inline
bool  is_empty_annotation(Annotation* annotation)
{
  return annotation == empty_annotation;
}


bool
Pattern::matches(Annotation* a) const
{
  ASet aset(0, false);
  aset.append_annotation(a);
  ExecEnv    env;
  return match(env, aset, 0);
}


bool
ConjPattern::match(ExecEnv&     env,
                   ASet&        aset,
                   position_t   beg_pos,
             const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  if (is_empty_pattern(gama)) {
    return p.match(env, aset, beg_pos, q);
  }
  else {
    ConjPattern new_gama(q, gama);
    return p.match(env, aset, beg_pos, new_gama);
  }
}


bool
DisjPattern::match(ExecEnv&     env,
                   ASet&        aset,
                   position_t   beg_pos,
             const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  if (p.match(env, aset, beg_pos, gama)) {
    return true;
  }
  else {
    return q.match(env, aset, beg_pos, gama);
  }
}


bool
LSpimPattern::match(ExecEnv&     env,
                    ASet&        aset,
                    position_t   beg_pos,
              const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  if (q.match(env, aset, beg_pos)) {
    return p.match(env, aset, beg_pos, gama);
  }
  else {
    return false;
  }
}


bool
RSpimPattern::match(ExecEnv&     env,
                    ASet&        aset,
                    position_t   beg_pos,
              const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  if (p.match(env, aset, beg_pos)) {
    return q.match(env, aset, beg_pos, gama);
  }
  else {
    return false;
  }
}


bool
IterPattern::match(ExecEnv&    env,
                   ASet&       aset,
                   position_t  beg_pos,
             const Pattern&    gama) const
{
  CHECK_PRECONDITIONS;

  if (min_occur > 0) {
    if (is_empty_pattern(gama)) {
      IterPattern new_gama(p, min_occur-1, max_occur-1);
      return p.match(env, aset, beg_pos, new_gama);
    }
    else {
      IterPattern ip(p, min_occur-1, max_occur-1);
      ConjPattern new_gama(ip, gama);
      return p.match(env, aset, beg_pos, new_gama);
    }
  }
  if (max_occur > 0) {
    if (is_empty_pattern(gama)) {
      IterPattern new_gama(p, 0, max_occur-1);
      if (p.match(env, aset, beg_pos, new_gama)) {
        return true;
      }
    }
    else {
      IterPattern ip(p, 0, max_occur-1);
      ConjPattern new_gama(ip, gama);    
      if (p.match(env, aset, beg_pos, new_gama)) {
        return true;
      }
    }
  }
  if (is_empty_pattern(gama)) {
    return true;
  }
  else {
    return gama.match(env, aset, beg_pos);
  }
}


class PhanGamaPattern : public UnaryOperatorPattern {
  position_t _beg_pos;
public:

  PhanGamaPattern(position_t beg_pos, const Pattern& p)
    : UnaryOperatorPattern(p)
    , _beg_pos(beg_pos)
  {;}

  bool  match(ExecEnv&     env,
              ASet&        aset,
              position_t /*beg_pos*/,
        const Pattern&     gama) const
  {
    env.set_end_pos(_beg_pos);
    if (is_empty_pattern(p)) {
      assert(is_empty_pattern(gama));
      return true;
    }
    else {
      return p.match(env, aset, _beg_pos, gama);
    }
  }
};

bool
PhanPattern::match(ExecEnv&     env,
                   ASet&        aset,
                   position_t   beg_pos,
             const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  PhanGamaPattern new_gama(beg_pos, gama);
  return p.match(env, aset, beg_pos, new_gama);
}


class LabelGamaPattern : public UnaryOperatorPattern {
  Label& _lbl;
public:

  LabelGamaPattern(Label& lbl, const Pattern& p)
    : UnaryOperatorPattern(p)
    , _lbl(lbl)
  {;}

  bool  match(ExecEnv&     env,
              ASet&        aset,
              position_t   beg_pos,
        const Pattern&     gama) const
  {
    _lbl._end_pos = env.end_pos();
    _lbl._annotation = env.annotation();
    if (is_empty_pattern(p)) {
      return true;
    }
    else {
      return p.match(env, aset, beg_pos, gama);
    }    
  }
};

bool
LabelPattern::match(ExecEnv&    env,
                    ASet&       aset,
                    position_t  beg_pos,
              const Pattern&    gama) const
{
  CHECK_PRECONDITIONS;

  Label& lbl = env.label(lbl_idx);
  lbl._aset       = &aset; 
  lbl._pattern    = &p;
  lbl._beg_pos    = beg_pos;
  lbl._annotation = 0;
  LabelGamaPattern new_gama(lbl, gama);
  return p.match(env, aset, beg_pos, new_gama);
}


class NegaGamaPattern : public UnaryOperatorPattern {
public:
  NegaGamaPattern(const Pattern& p)
    : UnaryOperatorPattern(p)
  {;}

  bool  match(ExecEnv&    /*env*/,
              ASet&       /*aset*/,
              position_t  /*beg_pos*/,
        const Pattern&    /*gama*/) const
  {
    assert(0);
    return false;
  }
};

bool
NegaPattern::match(ExecEnv&     env,
                   ASet&        aset,
                   position_t   beg_pos,
             const Pattern&     gama) const
{
  CHECK_PRECONDITIONS;

  if (const DisjPattern* o = dynamic_cast<const DisjPattern*>(&p)) {
    // ~(A|B) <=> ~A\~B
    NegaPattern np(o->p);
    if (np.match(env, aset, beg_pos, empty_pattern)) {
      NegaPattern nq(o->q);
      return nq.match(env, aset, beg_pos, gama);
    }
    return false;
  }
  if (const SpimPattern* o = dynamic_cast<const SpimPattern*>(&p)) {
    // ~(A/B) <=> ~A|~B
    {
      NegaPattern np(o->p);
      if (np.match(env, aset, beg_pos, gama)) {
        return true;
      }
    }
    NegaPattern nq(o->q);
    return nq.match(env, aset, beg_pos, gama);
  }
  if (const ConjPattern* o = dynamic_cast<const ConjPattern*>(&p)) {
    // ~(AB) <=> ~A|A~B
    {
      NegaPattern np(o->p);
      if (np.match(env, aset, beg_pos, gama)) {
        return true;
      }
    }
    NegaPattern nq(o->q);
    if (is_empty_pattern(gama)) {
      return o->p.match(env, aset, beg_pos, nq);
    }
    else {
      ConjPattern new_gama(nq, gama);
      return o->p.match(env, aset, beg_pos, new_gama);
    }
  }
  if (dynamic_cast<const BasicPattern*>(&p)) {
    NegaGamaPattern new_gama(gama);
    return p.match(env, aset, beg_pos, new_gama);
  }

  assert(0);
  return false;
}


bool
BasicPattern::match(ExecEnv&    env,
                    ASet&       aset,
                    position_t  beg_pos,
              const Pattern&    gama) const
{
  CHECK_PRECONDITIONS;

  if (beg_pos != aset.npos()) {
    AIterator ai = aset.get_annotations(mask, beg_pos);
    while (Annotation* a = ai.next()) {
      if (fast_match || match(*a)) {
        position_t end_pos = beg_pos + a->size();
        env.set_end_pos(end_pos);
        env.set_annotation(a);
        if (!is_empty_pattern(gama)) {
          if (dynamic_cast<const NegaGamaPattern*>(&gama)) {
            return false;
          }
          position_t next_pos = env.next_minded_pos(aset, end_pos);
          if (gama.match(env, aset, next_pos)) {
            return true;
          }
          // else - go on and try rest matches (if any)
        }
        else {
          return true;
        }
      }
    }
  }
  if (const NegaGamaPattern* n = dynamic_cast<const NegaGamaPattern*>(&gama)) {
    if (is_empty_pattern(n->p)) {
      return true;
    }
    else {
      return n->p.match(env, aset, beg_pos);
    }
  }
  else {
    return false;
  }
}


bool
ActionPattern::match(ExecEnv&    env,
                     ASet&       aset,
                     position_t  beg_pos,
               const Pattern&    gama) const
{
  CHECK_PRECONDITIONS;

  Annotation* a = action(env, aset, beg_pos);

  if (a == 0) {
    if (const NegaGamaPattern* n = dynamic_cast<const NegaGamaPattern*>(&gama)) {
      if (is_empty_pattern(n->p)) {
        return true;
      }
      else {
        return n->p.match(env, aset, beg_pos);
      }
    }
    else {
      return false;
    }
  }

  position_t end_pos;
  if (!is_empty_annotation(a)) {
#ifndef NDEBUG
    AIterator ai = aset.get_annotations(a->type(), beg_pos);
    Annotation* aa;
    while (0 != (aa = ai.next())) {
      if (aa == a) {
        break;
      }
    }
    assert(aa != 0);
#endif
    end_pos = beg_pos + a->size();
    env.set_end_pos(end_pos);
    env.set_annotation(a);
  }
  else {
    end_pos = beg_pos;
  }

  if (is_empty_pattern(gama)) {
    return true;
  }

  position_t next_pos;
  if (beg_pos == end_pos) {
    next_pos = end_pos;
  }
  else {
    assert(beg_pos < end_pos && end_pos <= aset.end_pos());
    next_pos = env.next_minded_pos(aset, end_pos);
  }
  return gama.match(env, aset, next_pos);
}
