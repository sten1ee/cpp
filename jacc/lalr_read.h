#ifndef lalr_read_h
#define lalr_read_h

class Grammar;

bool  read_grammar(std::istream& is, Grammar& grammar, std::ostream& erros);

#endif //lalr_read_h
