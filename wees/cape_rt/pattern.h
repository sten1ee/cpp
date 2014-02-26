#ifndef CAPE_PATTERN_H
#define CAPE_PATTERN_H

#include "annotation.h"
#include <assert.h>

class ExecEnv;

#ifndef NDEBUG
#   define DEBUG_PATTERNS
#endif

extern const class Pattern&          empty_pattern;
extern       class Annotation* const empty_annotation;

class Pattern {
public:

  virtual ~Pattern() {;}

  // If the pattern fails to match false is returned.
  // If sucessfully matched true is returned.
  virtual bool  match(ExecEnv&    env,
                      ASet&       aset,
                      position_t  beg_pos,
                const Pattern&    gama=empty_pattern) const=0;

  // Test whether this pattern matches a.
  bool  matches(Annotation* a) const;

#ifdef DEBUG_PATTERNS
  mutable const char* _dbg_string;

  Pattern() : _dbg_string("N/A") {;}

  class DbgStringLinker {
  public:
    DbgStringLinker(const Pattern& pat, const char* dbg_string)
    {
      pat._dbg_string = dbg_string;
    }
  };

  friend class DbgStringLinker;

#   define DBG_STRING(PAT, STR) Pattern::DbgStringLinker PAT##dsl(PAT, STR)
#else
#   define DBG_STRING(PAT, STR)
#endif

};


class OperatorPattern : public Pattern {
};


class BinaryOperatorPattern : public OperatorPattern {
public:
  const Pattern& p;
  const Pattern& q;

  BinaryOperatorPattern(const Pattern& p, const Pattern& q)
    : p(p), q(q)
  {;}
};


class ConjPattern : public BinaryOperatorPattern {
public:

  ConjPattern(const Pattern& p, const Pattern& q)
    : BinaryOperatorPattern(p, q)
  {;}

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class DisjPattern : public BinaryOperatorPattern {
public:

  DisjPattern(const Pattern& p, const Pattern& q)
    : BinaryOperatorPattern(p, q)
  {;}

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class SpimPattern : public BinaryOperatorPattern {
public:

  SpimPattern(const Pattern& p, const Pattern& q)
    : BinaryOperatorPattern(p, q)
  {;}
};


class LSpimPattern : public SpimPattern {
public:

  LSpimPattern(const Pattern& p, const Pattern& q)
    : SpimPattern(p, q)
  {;}

  // Succeeds iff p and q both succeed to match from beg_pos.
  // In this case the results from the p.match are in effect.
  // LSpim comes form "Left SuPerIMposition"
  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class RSpimPattern : public SpimPattern {
public:

  RSpimPattern(const Pattern& p, const Pattern& q)
    : SpimPattern(p, q)
  {;}

  // Succeeds iff p and q both succeed to match from beg_pos.
  // In this case the results from the q.match are in effect.
  // RSpim comes form "Right SuPerIMposition"
  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class UnaryOperatorPattern : public OperatorPattern {
public:
  const Pattern& p;

  UnaryOperatorPattern(const Pattern& p)
    : p(p)
  {;}
};


class IterPattern : public UnaryOperatorPattern {
public:
  const size_t    min_occur;
  const size_t    max_occur;

  IterPattern(const Pattern& p, size_t min_occur, size_t max_occur=size_t(-1))
    : UnaryOperatorPattern(p), min_occur(min_occur), max_occur(max_occur)
  {
    assert(min_occur <= max_occur);
  }

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class PhanPattern : public UnaryOperatorPattern {
public:

  PhanPattern(const Pattern& p)
    : UnaryOperatorPattern(p)
  {;}

  // Succeeds <=> p.match(...) succeeds.
  // In this case env.end_pos is set to beg_pos.
  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class NegaPattern : public UnaryOperatorPattern {
public:

  NegaPattern(const Pattern& p)
    : UnaryOperatorPattern(p)
  {;}

  // Succeeds <=> p.match(...) fails.
  // In this case env.end_pos is set to beg_pos.
  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class Label {
private:
  ASet*          _aset;
  const Pattern* _pattern;
  Annotation*    _annotation;
  position_t     _beg_pos;
  position_t     _end_pos;

  friend class LabelPattern;
  friend class LabelGamaPattern;

public:
  int X;
  int Y;
  int Z;

  Label() : _aset(0), _pattern(0), _annotation(0),
            _beg_pos(0), _end_pos(0),
            X(-1), Y(-1), Z(-1) {;}

  void  remove()
  {
    if (_annotation->type() == 0) {
      assert(false);
      return;
    }
    _aset->remove_annotation(_annotation, _beg_pos);
    _annotation = 0;
  }

  void  add(Annotation* a)
  {
    _aset->add_annotation(a, _beg_pos, _end_pos);
    _annotation = a;
  }

  position_t  beg_pos() const { return _beg_pos; }
  position_t  end_pos() const { return _end_pos; }
  position_t  size()    const { return _end_pos - _beg_pos; }

  const ASet&       aset()       const { return *_aset; }
  const Pattern&    pattern()    const { return *_pattern; }
        Annotation* annotation() const { return _annotation; }

  // Same as above
  // but the result is properly downcast.
  template <class ANNOTATION>
  ANNOTATION* a() const
  {
    assert(dynamic_cast<ANNOTATION*>(_annotation));
    return static_cast<ANNOTATION*>(_annotation);
  }
};


class LabelPattern : public UnaryOperatorPattern {
public:
  const int   lbl_idx;

  LabelPattern(const Pattern& p, int lbl_idx)
    : UnaryOperatorPattern(p), lbl_idx(lbl_idx)
  {;}

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;
};


class BasicPattern : public Pattern {
public:
  // first a fast mask-match is performed against the target annotation
  // type and if successful - the match(...) method is invoked.
  const atype_t  mask;

  // when fast_match == true - only the mask is matched
  // and the match(...) method is not invoked.
  const bool     fast_match;

  BasicPattern(atype_t mask, bool fast_match=false)
    : mask(mask), fast_match(fast_match)
  {;}

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;

  virtual bool  match(Annotation& a)const=0;
};


class ActionPattern : public Pattern {
public:

  ActionPattern() {;}

  bool  match(ExecEnv&    env,
              ASet&       aset,
              position_t  beg_pos,
        const Pattern&    gama) const;

  virtual Annotation* action(ExecEnv&    env,
                             ASet&       aset,
                             position_t  beg_pos) const=0;
};

#endif//CAPE_PATTERN_H