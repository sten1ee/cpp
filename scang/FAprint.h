#ifndef PG_FAPRINT_H
#define PG_FAPRINT_H

class ostream;
class FA;
class TGroup;
class TGroupMgr;

void  PrintFA(const FA* fa, ostream& os);
void  PrintGroup(const TGroup& group, ostream& os);
void  PrintGroups(const TGroupMgr& tgm, ostream& os);

#endif//PG_FAPRINT_H
