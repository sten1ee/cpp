#include "errnote.h"
#include "atypes.h"


void  ErrNote::add_modif(position_t beg_csi, position_t end_csi, const char* txt)
{
  assert(_modifs.empty()
     || type == REPLACE_ONE && _modifs.last()->data().beg_csi == beg_csi
                            && _modifs.last()->data().end_csi == end_csi
     || _modifs.last()->data().end_csi <= beg_csi);
  _modifs.add(Modif(beg_csi, end_csi, txt));
}

//-----------------------------------------------------------------------------

void  ErrNote::replace(position_t beg_csi, position_t end_csi, const char* new_txt)
{
  assert(type == REPLACE_ALL || type == REPLACE_ONE);
  add_modif(beg_csi, end_csi, new_txt);
}


void  ErrNote::replace(const AtToken& tok, const char* new_txt)
{
  assert(type == REPLACE_ALL || type == REPLACE_ONE);
  add_modif(tok.beg_csi(), tok.end_csi(), new_txt);
}


void  ErrNote::replace(const AtToken& p, const AtToken& q, const char* new_txt)
{
  assert(type == REPLACE_ALL || type == REPLACE_ONE);
  add_modif(p.beg_csi(), q.end_csi(), new_txt);
}

//-----------------------------------------------------------------------------

void  ErrNote::del(position_t beg_csi, position_t end_csi)
{
  assert(type == DELETE_ALL);
  add_modif(beg_csi, end_csi, 0);
}


void  ErrNote::del(const AtToken& tok)
{
  assert(type == DELETE_ALL);
  add_modif(tok.beg_csi(), tok.end_csi(), 0);
}


void  ErrNote::del(const AtToken& p, const AtToken& q)
{
  assert(type == DELETE_ALL);
  add_modif(p.beg_csi(), q.end_csi(), 0);
}

//-----------------------------------------------------------------------------

void  ErrNote::mark(position_t beg_csi, position_t end_csi)
{
  assert(type == MSG_ONLY);
  add_modif(beg_csi, end_csi, 0);
}

void  ErrNote::mark(const AtToken& tok)
{
  assert(type == MSG_ONLY);
  add_modif(tok.beg_csi(), tok.end_csi(), 0);
}

void  ErrNote::mark(const AtToken& p, const AtToken& q)
{
  assert(type == MSG_ONLY);
  add_modif(p.beg_csi(), q.end_csi(), 0);
}

//-----------------------------------------------------------------------------

const char* ErrNote::type_name(type_t type)
{
  switch (type) {
  case MSG_ONLY    : return "MSG_ONLY";
  case NONE        : return "NONE";
  case REPLACE_ONE : return "REPLACE_ONE";
  case REPLACE_ALL : return "REPLACE_ALL";
  case DELETE_ALL  : return "DELETE_ALL";
  default          : assert(false);
                     return "unknown!!!";
  }
}
