#ifndef CAPE_EXECENV_H
#define CAPE_EXECENV_H

#include "annotation.h"
#include "pattern.h"

class ExecEnv;
typedef int   phase_res_type;
typedef int (*phase_func_type)(ExecEnv& env, ASet& aset, position_t beg_pos, position_t end_pos);

class CAPE_API ExecEnv {
  atype_t         _mind_mask;
  Label*          _labels;
  int             _n_labels;
  Annotation*     _annotation;
  position_t      _end_pos;

public:
  ExecEnv() 
    : _mind_mask(atype_t(0)),
      _labels(0), _n_labels(0),
      _annotation(0),
      _end_pos(0)
  {;}

 ~ExecEnv()
  {;}

  // Sets the Label(s) array to be used by LabelPattern(s).
  void  use_labels(Label* labels, int n_labels)
  {
    _labels = labels;
    _n_labels = n_labels;
  }

  // Provides access to the Label(s) array in use.
  Label&  label(int idx)
  {
    assert(0 <= idx && idx < _n_labels);
    return _labels[idx];
  }

  // Sets the atype mask for the minded annotations.
  void  mind(atype_t mind_mask)
  {
    _mind_mask = mind_mask;
  }

  // Saves the context (mind mask & label set) executes the given phase
  // and restores the context.
  phase_res_type  exec(phase_func_type  phase_func,
                       ASet&            aset,
                       position_t       beg_pos,
                       position_t       end_pos);

  // Seeks for the first minded pos that is >= beg_pos.
  // A 'minded' pos is the beg_pos of a minded annotation.
  // A 'minded' annotation is one that matches the _minded_mask.
  // If not found - aset.npos() is returned.
  position_t  next_minded_pos(ASet& aset, position_t beg_pos)
  {
    return aset.find_annotations(_mind_mask, beg_pos);
  }

  Annotation* annotation() const
  {
    return _annotation;
  }

  void  set_annotation(Annotation* annotation)
  {
    _annotation = annotation;
  }

  const position_t& end_pos() const
  {
    return _end_pos;
  }

  void  set_end_pos(position_t pos)
  {
    _end_pos = pos;
  }

#ifndef NDEBUG
  // Writes a msg to the log
  static void  logf(const char* fmt, ...);
#endif
};

#endif//CAPE_EXECENV_H