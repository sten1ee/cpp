#ifndef PG_FATRANSF_H
#define PG_FATRANSF_H

class FA;
class RE;

void  NFA_join_RE(FA* nfa, RE* re, int finalCode);
FA*   NFA_to_MDFA(FA* nfa);
FA*   MDFA_to_DFA(FA* mdfa);
FA*   DFA_to_OMDFA(FA* dfa);

#endif//PG_FATRANSF_H
