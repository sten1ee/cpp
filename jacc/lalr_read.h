#ifndef lalr_read_h
#define lalr_read_h

class istream;
class ostream;
class Grammar;

bool  read_grammar(istream& is, Grammar& grammar, ostream& erros);

#endif //lalr_read_h
