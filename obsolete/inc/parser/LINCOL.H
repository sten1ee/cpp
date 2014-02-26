#ifndef  PARSER_LINCOL_H
#define  PARSER_LINCOL_H

#include <iostream.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                             class LinCol
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class LinCol {
  protected:
    unsigned short _line;
    unsigned short _col;
  public:
    LinCol();
    LinCol(unsigned short  __line, unsigned short __col);

    void  inc_col ();
    void  inc_line();
    void  dec_col ();
    void  dec_line();
    void  add_col (unsigned short nc);
    void  add_line(unsigned short nl);

    unsigned  line() const;
    unsigned  col () const;

    void  set_line(unsigned short sl);
    void  set_col (unsigned short sc);

    LinCol& operator = (const LinCol& other);
};

inline
ostream& operator << (ostream& os, const LinCol& lincol)
  {
    return  os << "(line " << lincol.line() << ", col " << lincol.col() << ')';
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                 inline members of LinCol
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline
LinCol::LinCol()
  {
    _line = _col = 0;
  }

inline
LinCol::LinCol(unsigned short __line, unsigned short __col)
  {
    _line = __line;
    _col  = __col;
  }

inline
void  LinCol::inc_col()
  {
    _col++;
  }

inline
void  LinCol::inc_line()
  {
    _line++;
  }

inline
void  LinCol::dec_col()
  {
    _col--;
  }

inline
void  LinCol::dec_line()
  {
    _line--;
  }

inline
void  LinCol::add_col(unsigned short nc)
  {
    _col += nc;
  }

inline
void  LinCol::add_line(unsigned short nl)
  {
    _line += nl;
  }

inline
unsigned  LinCol::line() const
  {
    return _line;
  }

inline
unsigned  LinCol::col() const
  {
    return _col;
  }

inline
void  LinCol::set_line(unsigned short sl)
  {
    _line = sl;
  }

inline
void  LinCol::set_col(unsigned short sc)
  {
    _col = sc;
  }

inline
LinCol& LinCol::operator = (const LinCol& other)
  {
    _line = other._line;
    _col  = other._col;
    return *this;
  }

#endif //PARSER_LINCOL_H
