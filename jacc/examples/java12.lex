%{
#include "java12_sym.h"

int src_lineno = 1;

#define TT(LT) /*printf("%s\t`%s'\n", #LT, yytext);*/ return (int)new LT##_sym;
#define ERR(MSG) printf("line %d: %s\n", src_lineno, MSG);
%}

d     [[:digit:]]
xd    [[:xdigit:]]

%%

[ \t\f]*  /* skip blanks */
\n      src_lineno++;
"/*"    {
        int c;

        while((c = yyinput()) != 0)
            {
            if(c == '\n')
                ++src_lineno;

            else if(c == '*')
                {
                if((c = yyinput()) == '/')
                    break;
                else
                    unput(c);
                }
            }
        }

"//".*$

boolean    TT(BOOLEAN)
byte       TT(BYTE)
short      TT(SHORT)
int        TT(INT)
long       TT(LONG)
char       TT(CHAR)
float      TT(FLOAT)
double     TT(DOUBLE)
"["        TT(LBRACK)
"]"        TT(RBRACK)
"."        TT(DOT)
";"        TT(SEMICOLON)
"*"        TT(MULT)
","        TT(COMMA)
"{"        TT(LBRACE)
"}"        TT(RBRACE)
"="        TT(EQ)
"("        TT(LPAREN)
")"        TT(RPAREN)
":"        TT(COLON)
package    TT(PACKAGE)
import     TT(IMPORT)
public     TT(PUBLIC)
protected  TT(PROTECTED)
private    TT(PRIVATE)
static     TT(STATIC)
abstract   TT(ABSTRACT)
final      TT(FINAL)
native     TT(NATIVE)
synchronized TT(SYNCHRONIZED)
transient  TT(TRANSIENT)
volatile   TT(VOLATILE)
class      TT(CLASS)
extends    TT(EXTENDS)
implements TT(IMPLEMENTS)
void       TT(VOID)
throws     TT(THROWS)
this       TT(THIS)
super      TT(SUPER)
interface  TT(INTERFACE)
if         TT(IF)
else       TT(ELSE)
switch     TT(SWITCH)
case       TT(CASE)
default    TT(DEFAULT)
do         TT(DO)
while      TT(WHILE)
for        TT(FOR)
break      TT(BREAK)
continue   TT(CONTINUE)
return     TT(RETURN)
throw      TT(THROW)
try        TT(TRY)
catch      TT(CATCH)
finally    TT(FINALLY)
new        TT(NEW)
"++"       TT(PLUSPLUS)
"--"       TT(MINUSMINUS)
"+"        TT(PLUS)
"-"        TT(MINUS)
"~"        TT(COMP)
"!"        TT(NOT)
"/"        TT(DIV)
"%"        TT(MOD)
"<<"       TT(LSHIFT)
">>"       TT(RSHIFT)
">>>"      TT(URSHIFT)
"<"        TT(LT)
">"        TT(GT)
"<="       TT(LTEQ)
">="       TT(GTEQ)
instanceof TT(INSTANCEOF)
"=="       TT(EQEQ)
"!="       TT(NOTEQ)
"&"        TT(AND)
"^"        TT(XOR)
"|"        TT(OR)
"&&"       TT(ANDAND)
"||"       TT(OROR)
"?"        TT(QUESTION)
"*="       TT(MULTEQ)
"/="       TT(DIVEQ)
"%="       TT(MODEQ)
"+="       TT(PLUSEQ)
"-="       TT(MINUSEQ)
"<<="      TT(LSHIFTEQ)
">>="      TT(RSHIFTEQ)
">>>="     TT(URSHIFTEQ)
"&="       TT(ANDEQ)
"^="       TT(XOREQ)
"|="       TT(OREQ)

strictfp   TT(STRICTFP)
const      TT(CONST)
goto       TT(GOTO)
null       TT(NULL_LITERAL)


[0-9]+[lL]?                         TT(INTEGER_LITERAL)
0x{xd}+[lL]?                        TT(INTEGER_LITERAL)

{d}+\.{d}+([eE][-+]?{d}+)?[fFdD]?   TT(FLOATING_POINT_LITERAL)
\.{d}+([eE][-+]?{d}+)?[fFdD]?       TT(FLOATING_POINT_LITERAL)
{d}+\.([eE][-+]?{d}+)?[fFdD]?       TT(FLOATING_POINT_LITERAL)
{d}+([eE][-+]?{d}+)[fFdD]?          TT(FLOATING_POINT_LITERAL)

true|false                          TT(BOOLEAN_LITERAL)

\'([^\\\n']|\\.)+\'                 TT(CHARACTER_LITERAL)
\"([^\\\n"]|\\.)*\"                 TT(STRING_LITERAL)

\'([^\\\n']|\\.)*\n                {ERR("Unterminated string literal");
                                    src_lineno++;}
\"([^\\\n"]|\\.)*\n                {ERR("Unterminated character literal");
                                    src_lineno++;}
\'\'                                ERR("Empty character literal")

[[:alpha:]$_][[:alnum:]$_]*         TT(IDENTIFIER)
.                                  {printf("line %d: Illegal character `%c'\n",
                                            src_lineno, *yytext);}
<<EOF>>                             TT(eof)
%%
