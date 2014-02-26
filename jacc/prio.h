#ifndef PRIO_H
#define PRIO_H

const int PREC_MIN =    0;
const int PREC_MAX = 1000;

enum  assoc_t   { ASSOC_LEFT = 1, ASSOC_RIGHT = 2, ASSOC_NONE = 3 };

// Class Prio is an abstraction of the classic Precedence - Associativity pair
// There are two special Prio values
//   - NULL - the uninitialized Prio
//   - NAPP - the not applicable Prio
class Prio
{
  protected:
    int   value;

    Prio(int i) { value = i; }

  public:

    // constructs the null prio
    Prio() { value = 0; }

    // constructs a normal prio with the specified p&a
    Prio(int p, assoc_t a)
    {
      ASSERT(PREC_MIN <= p && p <= PREC_MAX);
      ASSERT(1 <= a && a <= 3);
      value = (p << 2) | a;
    }

    int       prec() const
    {
      ASSERT(is_normal());
      return value >> 2;
    }

    assoc_t   assoc() const
    {
      ASSERT(is_normal());
      return assoc_t(value & 3);
    }

    bool      is_null()   const { return value == 0; }
    bool      is_napp()   const { return value <  0; }
    bool      is_normal() const { return value >  0; }

    // 'constructor' for the NAPP prio:
    static Prio napp() { return Prio(-1); }
    // 'constructor' for the NULL prio:
    static Prio null() { return Prio(0);  }
};

#endif //PRIO_H
