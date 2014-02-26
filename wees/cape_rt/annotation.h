#ifndef CAPE_ANNOTATION_H
#define CAPE_ANNOTATION_H

#include "array.h"
#include <assert.h>

#ifndef CAPE_API
#define CAPE_API __declspec(dllimport)
#endif

enum atype_t;

typedef size_t  position_t;

const atype_t MAJOR_ATYPE = atype_t(0xff000000);
const atype_t MINOR_ATYPE = atype_t(0x00ffffff);

// Annotation
//
class CAPE_API Annotation {
public:

  virtual ~Annotation() {;}

  Annotation(atype_t type)
    : _type(type), _size(0)
  {;}

  atype_t  type() const { return _type; }
  size_t   size() const { return _size; }

  bool     matches(atype_t mask) const
  {
    assert(this);
    if (mask < 0) {
      mask = atype_t(~mask & MAJOR_ATYPE | mask & MINOR_ATYPE);
    }
    return 0 == (~mask & _type);
  }

private:
  friend class ASet;

  atype_t   _type;
  size_t    _size;
};

// ANode
//
struct CAPE_API ANode {
  Annotation* annotation;
  ANode*      next;

  static ANode* first_match_node(ANode* node, atype_t mask)
  {
    while (node && !node->annotation->matches(mask)) {
      node = node->next;
    }
    return node;
  }
};

// AIterator
//
class CAPE_API AIterator {
  ANode*  _cnode;
  atype_t _mask;

public:

  AIterator()
    : _cnode(0), _mask(atype_t(0))
  {;}

  AIterator(ANode* node, atype_t mask)
    : _cnode(node), _mask(mask)
  {;}

  // Returns a pointer to the next Annotation available or
  // NULL as an end-of-iteration indicator.
  // User should NOT delete (normally).
  Annotation* next()
  {
    ANode* match = ANode::first_match_node(_cnode, _mask);
    if (0 != match) {
      _cnode = match->next;
      return match->annotation;
    }
    else {
      _cnode = 0;
      return 0;
    }
  }

  // Same as above
  // but the result is properly downcast.
  template <class ANNOTATION>
  ANNOTATION* next()
  {    
    if (Annotation* a = next()) {
      assert(dynamic_cast<ANNOTATION*>(a));
      return static_cast<ANNOTATION*>(a);
    }
    else {
      return 0;
    }
  }
};

// ASet
//
class CAPE_API ASet {
  Array<ANode*>       _anodes;   // _anodes.size == _end_pos - _beg_pos
  const position_t    _base_pos; // associated with _anodes[0]
  // if true - deletes added annotations upon clear()
  bool                _del_annotations;

  // deletes all anodes
  void  clear();

public:
  ASet(position_t base_pos=0, bool del_annotations=true)
    : _base_pos(base_pos), _del_annotations(del_annotations)
  {
  }

 ~ASet()
  {
    clear();
  }

  // Gets the N/A possition - greater than every possible poition in the set.
  position_t    npos()    const { return position_t(-1); }

  // Gets the first available position within the position range of the set.
  position_t    beg_pos() const { return _base_pos; }
  
  // Gets the one-after-the last position within the position range of the set.
  position_t    end_pos() const { return _base_pos + _anodes.size(); }

  // Gets the number of positions available in this annotation set.
  size_t        size()    const { return _anodes.size(); }

  // Creates iterator over the annotations 
  // matching the given mask
  // and having the specified beginning position.
  // Returns an empty iterator iff
  // no annotation at that position matches the filter.
  AIterator  get_annotations(atype_t mask, position_t pos) const;

  // used as special values for the `step' parameter of the next method
  enum seek_t { SEEK_BACK = -2, SEEK_FWD = -1 };

  // Let seek = (int)end_pos
  // Let step = { -1 if seek == -2
  //            | +1 if seek >= -1 }
  // If  seek < 0 set end_pos = aset.end_pos()
  // Let S =
  // { x | x = (beg_pos + K * step) where K is a non-negative integer
  //       and x is a valid position within the position range of the set
  //       and x < end_pos
  //       and get_annotations(mask, x) returns a non-empty iterator }
  //
  // If S is the empty set then npos() is returned 
  // (and optionally the empty iterator is stored in *p_iter)
  // Otherwise ((step > 0) ? min(S) : max(S)) is returned 
  // (and optionally get_annotations(mask, (min/max)(S)) is stored in *p_iter).
  // Note: There are generally two different kinds of invocations:
  // 1. Backward or forward searches with search range respectively
  //    [0, beg_pos] or [beg_pos, end_pos()).
  //    (`end_pos' set to SEEK_BACK or SEEK_FWD (-2 and -1 resp.))
  //    Instead of converting seek_t to position_t - use the next method.
  // 2. Forward searches on additionally restricted range [beg_pos, end_pos).
  //    (`seek' >= 0 is set to one-after-the-last position to search).
  //    In this case beg_pos <= end_pos <= end_pos() is assumed.
  position_t  find_annotations(atype_t     mask,
                               position_t  beg_pos,
                               position_t  end_pos,
                               AIterator*  p_iter=0) const;

  position_t  find_annotations(atype_t     mask,
                               position_t  beg_pos,
                               seek_t      seek=SEEK_FWD,
                               AIterator*  p_iter=0) const
  {
    return find_annotations(mask, beg_pos, (position_t)seek, p_iter);
  }

  // Appends the specified basic annotation at the end of the set.
  // Keep in mind that basic annotations have size of 1.
  // The set takes ownership over the annotation.
  void      append_basic_annotation(Annotation* a);

  // Appends the specified annotation at the end of the set.
  // The annotation should have its size properly set.
  // The set takes ownership over the annotation.
  void      append_annotation(Annotation* a);

  // Inserts the specified annotation at the specified position in the set.
  // May invalidate active iterators over the same beginning pos.
  // The set takes ownership over the annotation.
  void      add_annotation(Annotation* a,
                           position_t beg_pos, position_t end_pos);

  // Removes the specified annotation from the specified position in the set.
  // Note: *this* annotation (i.e. exactly the same object)
  // should be on exactly that (beg_pos) position in the set.
  void      remove_annotation(Annotation* a, position_t beg_pos);

protected:
  void      set_annotation_size(Annotation* a, size_t size) { a->_size = size; }
};

inline
AIterator  ASet::get_annotations(atype_t mask, position_t pos) const
{
  size_t idx = pos - _base_pos;
  assert(idx < _anodes.size());
  return AIterator(_anodes[idx], mask);
}

#endif//CAPE_ANNOTATION_H