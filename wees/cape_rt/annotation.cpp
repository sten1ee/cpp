#include "annotation.h"


void  ASet::clear()
{
  for (size_t i = 0; i < _anodes.size(); ++i) {
    ANode* node = _anodes[i];
    if (node != 0) {
      _anodes[i] = 0;
      do {
        ANode* del = node;
        node = node->next;
        if (_del_annotations) {
          delete del->annotation;
        }
        delete del;
      }
      while (node != 0);
    }
  }
}

position_t  ASet::find_annotations(atype_t mask, position_t beg_pos, position_t end_pos, AIterator* p_iter) const
{
  int beg_idx = (int)(beg_pos - _base_pos);
  assert(0 <= beg_idx);
  assert(beg_idx <= (int)_anodes.size());
  int end_idx;
  int step = (int)end_pos;
  if (step < 0) {
    if (step == SEEK_FWD) {
      step = 1;
    }
    else {
      assert(step == SEEK_BACK);
      step = -1;
    }
    end_idx = (int)_anodes.size();
  }
  else { // step is not a step but an end_pos restriction
    end_idx = (int)(end_pos - _base_pos);
    assert(beg_idx <= end_idx);
    assert(end_idx <= (int)_anodes.size());
    step = 1;
  }

  for (int i = beg_idx; 0 <= i && i < end_idx; i += step) {
    if (ANode* match_node = ANode::first_match_node(_anodes[i], mask)) {
      if (0 != p_iter) {
        *p_iter = AIterator(match_node, mask);
      }
      return (size_t)i + _base_pos;
    }
  }
  if (0 != p_iter) {
    *p_iter = AIterator(0, mask);
  }
  return npos();
}

void  ASet::add_annotation(Annotation* a, position_t beg_pos, position_t end_pos)
{
  size_t beg_idx = beg_pos - _base_pos;
  size_t end_idx = end_pos - _base_pos;

  assert(beg_idx < end_idx && end_idx <= _anodes.size());
  
  set_annotation_size(a, end_idx - beg_idx);
  ANode* new_node = new ANode;
  new_node->annotation = a;
  new_node->next = _anodes[beg_idx];
  _anodes[beg_idx] = new_node;
}

void  ASet::append_basic_annotation(Annotation* a)
{
  set_annotation_size(a, 1);
  ANode* new_node = new ANode;
  new_node->annotation = a;
  new_node->next = 0;
  _anodes.push_back(new_node);
}

void  ASet::append_annotation(Annotation* a)
{
  assert(a->size() != 0);
  ANode* new_node = new ANode;
  new_node->annotation = a;
  new_node->next = 0;
  _anodes.push_back(new_node);
}

void  ASet::remove_annotation(Annotation* a, position_t beg_pos)
{
  size_t beg_idx = beg_pos - _base_pos;
  for (ANode** ppnode = &_anodes[beg_idx]; *ppnode != 0; ppnode = &(*ppnode)->next) {
    if ((*ppnode)->annotation == a) {
      ANode* del_node = *ppnode;
      *ppnode = del_node->next;
      if (_del_annotations) {
        delete a;
      }
      delete del_node;
      return;
    }
  }
  assert(false);
}
