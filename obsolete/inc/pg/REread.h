#ifndef PG_REREAD_H
#define PG_REREAD_H

class RE;
class istream;

RE*   ReadRE(istream& is);
void  ErrorReadRE(const char* errmsg);

#endif//PG_REREAD_H
