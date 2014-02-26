#include "util/avl_tree.h"


void  avl_tree_base::insert(link_type parent, link_type* ppchild,
                            link_type a)
{ 
  a->parent = parent;
  a->left   = 0;
  a->right  = 0;
  a->len    = 1;
  *ppchild  = a;

  if (ppchild == &leftmost()->left) {
    _leftmost() = a;
    if (empty()) {
      _rightmost() = a;
    }
  }
  else if (ppchild == &rightmost()->right) {
    _rightmost() = a;
  }

  ++_size();

  link_type  b;
  int        a_len = 1;
  while (0 != (b = a->parent)) {
    if (b->len == a->len) {
      bool      a_is_b_left = (a == b->left);
      link_type c           = (a_is_b_left ? b->right : b->left);
      int       c_len       = (c ? c->len : 0);
      if (a_len - c_len > 1) {
        rotate(b, a_is_b_left);
        break;
      }
      else { // level up
        a_len = ++(a = b)->len;
      }
    }
    else {
      break;
    }
  }
}


avl_node_base*  avl_tree_base::next(link_type node)
{
  if (node->right) {
    node = node->right;
    while (node->left) {
      node = node->left;
    }
    return node;
  }
  else {
    link_type y = node->parent;
    while (node != y->left) {
      node = y;
      y = y->parent;
    }
    return y;
  }
}


avl_node_base*  avl_tree_base::prev(link_type node)
{
  if (node->left) {
    node = node->left;
    while (node->right) {
      node = node->right;
    }
    return node;
  }
  else {
    link_type y = node->parent;
    while (node == y->left) {
      node = y;
      y = y->parent;
    }
    return y;
  }
}


void  avl_tree_base::erase(link_type del)
{
  UTIL_ASSERT(!is_header(del)); // can't remove header

  _size()--;

  if (del == rightmost()) {
    if (del == leftmost()) {
      UTIL_ASSERT(is_header(del->parent));
      flushed();
      return;
    }
    else {
      _rightmost() = prev(del);
    }
  }
  else if (del == leftmost()) {
    _leftmost() = next(del);
  }

  link_type  b, c;
  link_type  del_parent = del->parent;
  link_type& rpdel = del_parent->left == del ? del_parent->left
                                             : del_parent->right;
  link_type avl_node_base::*left;
  link_type avl_node_base::*right;

  int replace;
  if (del->left) {
    if (del->right) {
      replace = del->len %2 ? -1 : +1;
    }
    else {
      replace = -1;
    }
  }
  else {
    if (del->right) {
      replace = 1;
    }
    else {
      replace = 0;
      rpdel = 0;
      b = del_parent;
      c = 0;
    }
  }

  if (replace) {
    if (replace < 0) {
      left  = &avl_node_base::left;
      right = &avl_node_base::right;
    }
    else {
      left  = &avl_node_base::right;
      right = &avl_node_base::left;
    }

    link_type rep = del->*left;
    while (rep->*right) {
      rep = rep->*right;
    }
    rep->*right = del->*right;

    link_type rep_parent = rep->parent;
    if (rep_parent != del) {
      UTIL_ASSERT(rep_parent->*right == rep);
      b = rep_parent;
      if (0 != (rep_parent->*right = c = rep->*left)) {
        (rep->*left)->parent = rep_parent;
      }
      (rep->*left = del->*left)->parent = rep;
    }
    else {
      c = (b = rep)->*left;
    }

    if (del->*right) {
      (del->*right)->parent = rep;
    }
    // del->parent->left/right <=> rep:
    rpdel = rep;
    rep->parent = del_parent;
    rep->len = del->len;
  }

  // from now on we no longer need 'del', but it can not be deleted here
  // since the proper destructor (~avl_node<DATA>) is not known here.

  int c_len = c ? c->len : 0;
  while (!is_header(b)) {
    bool      c_is_b_left = (c == b->left);
    link_type a           = (c_is_b_left ? b->right : b->left);
    int       a_len       = (a ? a->len : 0);
                                    //  b
                                    // a c <-
    if (b->len - c_len == 2) {
      if (a_len == c_len) {         //  2
        b->len--;                   // 0 0 <-
      }
      else {                        //  2
        UTIL_ASSERT(a_len > c_len); // 1 0 <-
        break;
      }
    }
    else {                              //  3
      UTIL_ASSERT(b->len - c_len == 3); // 2 0 <-
      int b_old_len = b->len;
      b = rotate(b, !c_is_b_left);
      if (b->len == b_old_len) {   // stablilized
        break;
      }
    }
    b = (c = b)->parent;
    c_len = c->len;
  }
}


avl_node_base*  avl_tree_base::rotate(link_type b, bool rot_right)
{
  link_type avl_node_base::*left;
  link_type avl_node_base::*right;

  if (rot_right) {
    left  = &avl_node_base::left;
    right = &avl_node_base::right;
  }
  else {
    left  = &avl_node_base::right;
    right = &avl_node_base::left;
  }
  link_type c = b->*right;
  link_type a = b->*left;
  link_type p = a->*left;
  link_type q = a->*right;

  UTIL_ASSERT(c && a->len - c->len == 2 || a->len == 2);

  if (!q || p && p->len > q->len) {
  SIMPLE_CASE:
    b->*left = q;
    if (q) {
      (q->parent = b)->len = q->len + 1;
    }
    else {
      b->len = 1;
    }
    a->*right = b;
    a->len = b->len + 1; //needed for spec case reduced to simple
    link_type parent = a->parent = b->parent;
    (parent->left == b ? parent->left : parent->right) = a;
    b->parent = a;
    return a;
  }
  else {
    link_type r = q->*left;
    if (p && p->len == q->len) { //special case of underflow
      int r_len = r ? r->len : 0;
      if (p->len - r_len == 2) {
        goto SIMPLE_CASE;
      }
      UTIL_ASSERT(p->len - r_len == 1);
      q->len++;
    }
    link_type s = q->*right;
    a->*right = r;
    if (r) {
      r->parent = a;
    }
    b->*left = s;
    if (s) {
      s->parent = b;
    }
    q->*left  = a;
    q->*right = b;
    a->parent = q;
    link_type parent = q->parent = b->parent;
    (parent->left == b ? parent->left : parent->right) = q;
    b->parent = q;
    b->len = (c ? c->len : 0) + 1;
    a->len = (p ? p->len : 0) + 1;
    q->len++;
    return q;
  }
}
