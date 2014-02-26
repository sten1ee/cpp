
template <class H, class T> struct Typelist {};

// NullList is an insipid type put by convention at the end of a typelist:
struct NullList {};

// You can build typelists of any length by composing Typelist with itself:
typedef Typelist<char, Typelist<signed char, Typelist<unsigned char, NullList> > > CharTypes;

// Now say you want to find a type at a given position in a typelist.
// You can do that with the following way:
template <class TList, unsigned int index> struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0>
{
     typedef Head Result;
};

template <unsigned int index>
struct TypeAt<NullList, index>
{
     typedef void Result;
};

template <class Head, class Tail, unsigned int i>
struct TypeAt<Typelist<Head, Tail>, i>
{
     typedef typename TypeAt<Tail, i - 1>::Result Result;
};

TypeAt<CharTypes, 0>::Result  v1 = 'c';
TypeAt<CharTypes, 1>::Result  v2 = 'c';
TypeAt<CharTypes, 2>::Result  v3 = 'c';
TypeAt<CharTypes, 3>::Result  v4 = 'c';