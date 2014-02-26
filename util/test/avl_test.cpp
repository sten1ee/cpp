#define  DEBUGGER_DECL  friend class DBG;
#define  AVL_TREE_SETOPER
#include "util/avl_tree.h"
#include "util/utility.h"
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

//using namespace std;

#define MIN_W 5

#ifndef max
#  ifdef __GNUC__
#    define max(X,Y) ((X) >? (Y))
#  endif
#endif

struct  ADT
{
  int   i;
  int   w;
  bool  ping;

  ADT(int _i) : i(_i), ping(false) {;}
};


struct ADT_Traits : public avl_traits<ADT>
{
  typedef   int   DataKey;

  static  int keyof(DataIn a) { return a.i; }

  static  int compare(DataKey i, DataKey j) { return  i - j; }
};

typedef avl_node<ADT_Traits> data_node;
typedef avl_tree<ADT_Traits> data_tree;

struct  named_tree : public data_tree
{
  char  name;

  named_tree(char c) : name(c) {;}
};

struct  named_tree_avl_traits : public avl_traits<named_tree>
{
  static int  compare(char c, DataIn tree) { return c - tree.name; }
  static int  compare(DataIn t1, DataIn t2) { return t1.name - t2.name; }
};

typedef avl_tree<named_tree_avl_traits> tree_dict;

struct DBG
{
  static  void            test();
  static  int             spread(avl_node_base* node, int w);
  static  ostream&        print(ostream& os, data_tree& tree);
  static  int             depth(avl_node_base* node);
  static  avl_node_base*  find_inconsistent_node(avl_node_base* node);
};

int  DBG::depth(avl_node_base* node)
{
  if (node == 0) {
    return 0;
  }
  else {
    int  d = 1 + max(depth(node->left), depth(node->right));
    ASSERT(d == node->len);
    return d;
  }
}

avl_node_base*  DBG::find_inconsistent_node(avl_node_base* node)
{
  if (!node) {
    return 0;
  }
  avl_node_base* bad;
  bad = find_inconsistent_node(node->left);
  if (bad) {
    return bad;
  }
  bad = find_inconsistent_node(node->right);
  if (bad) {
    return bad;
  }

  int diff = depth(node->left) - depth(node->left);
  if (1 < abs(diff)) {
    return node;
  }

  return 0;
}

int  DBG::spread(avl_node_base* node, int w)
{
  if (node == 0) {
    return w;
  }
  w = spread(node->left, w);
  ((data_node*)node)->data.w = w;
  w = spread(node->right, w + MIN_W);

  return w;
}

ostream&  DBG::print(ostream& os, data_tree& tree)
{
  if (!tree.root()) {
    return  os << "\n{<empty-tree>}";
  }

  spread(tree.root(), 0);

  for (int i = tree.root()->len; 0 < i; i--) {
    int pos = 0;
    os << '\n';
    for (data_tree::iterator data  = tree.begin();
                             data != tree.end();
                           ++data) {
      if (data.node->len == i) {
        while (pos < (*data).w) {
          os << ' ';
          pos++;
        }
        char  buf[16];
        pos += sprintf(buf, ((*data).ping ? "%d<%d>" : "%d[%d]"),
                             (*data).i, data.node->len);
        os << buf;
      }
    }
  }
  return os;
}


void  DBG::test()
{
  ostream& os = cout;
  tree_dict   dict;
  dict.insert(named_tree('a'));
  data_tree   tree = *dict.find('a');
  data_tree::iterator  iter = tree.begin();
  ASSERT(iter == tree.end());

  while (true) {
    cout << "\n?<";
    int c;
    switch (c = getche()) {
      case '[': if (iter != tree.end()) {
                  (*iter).ping = false;
                }
                if (--iter != tree.end()) {
                  (*iter).ping = true;
                }
                break;
      case ']': if (iter != tree.end()) {
                  (*iter).ping = false;
                }
                if (++iter != tree.end()) {
                  (*iter).ping = true;
                }
                break;
      case 'e': if (iter != tree.end()) {
                  tree.erase(iter++);
                }
                if (iter != tree.end()) {
                  (*iter).ping = true;
                }
                break;
      case 'f':
              {
                int n = 0, d;
                while (isdigit(d = getche())) {
                  n *= 10;
                  n += d - '0';
                }
                data_tree::finger f = tree.find(n);
                if (f) {
                  os << "found !";
                  if (iter != tree.end()) {
                    (*iter).ping = false;
                  }
                  (*(iter = f)).ping = true;

                }
                else {
                  os << "not found !";
                }
                break;
              }
      case 'g':
              {
                int n = 0, d;
                while (isdigit(d = getche())) {
                  n *= 10;
                  n += d - '0';
                }
                data_tree::finger g = tree.find(n);
                if (g) {
                  os << "found !";
                }
                else {
                  tree.insert(g, n);
                  os << "inserted !";
                }
                if (iter != tree.end()) {
                  (*iter).ping = false;
                }
                (*(iter = g)).ping = true;
                break;
              }
      case 'b':
              {
                int n = 0, d;
                while (isdigit(d = getche())) {
                  n *= 10;
                  n += d - '0';
                }
                tree.insert_back(n);
                break;
              }
      case 'q': return;
      case 't': os << "tree functions:"
                      "\n\t n -> new tree"
                      "\n\t d -> del tree"
                      "\n\t o -> operation"
                      "\n\t c -> change active tree";
                break;
      default : if (isdigit(c)) {
                  int n = 0;
                  do {
                    n *= 10;
                    n += c - '0';
                  } while (isdigit(c = getche()));
                  tree.insert(ADT(n));
                }
                else {
                  cout << "\n\t{      ->  ++iter"
                          "\n\t}      ->  --iter"
                          "\n\t<num>  ->  insert <num>"
                          "\n\tf<num> ->  finger find <num>"
                          "\n\tg<num> ->  finger insert <num>"
                          "\n\te      ->  erase current of iter"
                          "\n\tq      ->  quit";
                }
                break;
    }
    os << "\n!> ";
    print(os, tree);
    os << "\nsize: " << tree.size();
    if (iter == tree.end()) {
      os << "\n (iter at end)";
    }
    else {
      os << "\n (iter = " << (*iter).i << ")";
    }
    avl_node_base* bad = find_inconsistent_node(tree.root());
    if (bad) {
      os << "\n!!! Inconsistent node found : " << ((data_node*)bad)->data.i
         << "!!!\n";
    }
    os << "\n----------------------------------------------";
  }
}


int  main()
{
  do {
    DBG::test();
    cout << "\n?? another test (*/q) ?";
  } while (getche() != 'q');

  data_tree  p, q;
  p | q;
  is_subset(p, q);
  is_equal(p, q);

  return 0;
}
