#ifndef CAPE_ERRNOTE_H
#define CAPE_ERRNOTE_H

#include "list.h"
#include "phantomstr.h"
#include "annotation.h"


/** Describes a substitution of a continuous slice of text
 *  in the interval [beg_pos, end_pos) with the new_text.
 */
struct CAPE_API Modif {
  const position_t   beg_csi;
  const position_t   end_csi;
  const PhantomStr   new_txt;

 ~Modif()
  {;}

  Modif(position_t beg_csi, position_t end_csi, const char* new_txt)
    : beg_csi(beg_csi), end_csi(end_csi), new_txt(new_txt)
  {}

  typedef List<Modif> List;
};

class AtToken;

class CAPE_API ErrNote {
public:
  enum type_t {
    MSG_ONLY    = 0,
    NONE        = 1,
    REPLACE_ONE = 2,
    REPLACE_ALL = 3,
//  DELETE_ONE  = 4,
    DELETE_ALL  = 5,
//  INSERT_ONE  = 6,
//  INSERT_ALL  = 7
  };

  ErrNote(int id, type_t type, const PhantomStr& lbl)
    : id(id), type(type), label(lbl)
  {;}

  const int          id;    // id specific for this type of errors
  const type_t       type;  // one of MSG_ONLY, REPLACE_ONE, REPLACE_ALL, DELETE_ALL
  const PhantomStr   label; // something human-readable (description)

  typedef List<ErrNote> List;

private:
  Modif::List _modifs;

public:
  void  add_modif(position_t beg_csi, position_t end_csi, const char* txt);

  // len == 0 indicates that txt is a static string literal
  // otherwise txt should be copied and len is its actual length
  // void  add_insert(position_t pos, const char* txt)

  // replaces the [beg_csi, end_csi) interval with the given text
  void  replace(position_t beg_csi, position_t end_csi, const char* new_txt);

  // replaces a single token with the given text
  void  replace(const AtToken& tok, const char* new_txt);

  // replaces all the tokens between p and q (incl.) with the given text
  void  replace(const AtToken& p, const AtToken& q, const char* new_txt);

  // deletes the [beg_csi, end_csi) interval
  void  del(position_t beg_csi, position_t end_csi);

  // deletes a single token
  void  del(const AtToken& tok);

  // deletes all the tokens between p and q (incl.)
  void  del(const AtToken& p, const AtToken& q);

  // marks the [beg_csi, end_csi) interval
  void  mark(position_t beg_csi, position_t end_csi);

  // marks a single token
  void  mark(const AtToken& tok);

  // marks all the tokens between p and q (incl.)
  void  mark(const AtToken& p, const AtToken& q);

  Modif::List::Node* first_modif() const
  {
    return _modifs.first();
  }

  const Modif::List& modifs() const
  {
    return _modifs;
  }

  static const char* type_name(type_t type);
  
  const char* type_name() const
  {
    return type_name(type);
  }
};

#endif