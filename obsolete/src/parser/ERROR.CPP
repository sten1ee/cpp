#include <iostream.h>
#include "parser/error.h"
#include "parser/lincol.h"
#include "parser/debug.h"


const char* strErrorMessageEnglish[] =
{
  "???",
  "Unexpected EOF",
  "Int const out of range",
  "Long const out of range",
  "Float const out of precision",
  "Double const out of precision",
  "Bad number format",
  "Bad hex-number format",
  "Bad oct-number format",
  "Bad float-number format",
  "Comment not closed",
  "Unterminated string",
  "Unterminated char",
  "Bad hex-char format",
  "Bad uni-char format",
  "Bad escape sequence",
  "Illegal character",

  "'new' missing class id",
  "Argument list missing '('",
  "Argument list missing ','",
  "Argument list missing ')'",
  "Missing ']'",
  "Missing ')'",
  "Function name expected",
  "Ident expected on left of dot",
  "Ident expected on right of dot",
  "Typeid expected on left of []",
  "Expression expected but EOF found",
  "Term expected",
  "Array new: invalid dimension",
  "'?' missing ':'",
  "Array initializer missing '}'",
  "Bad initializer",

  "'if' missing '('",
  "'if' missing ')'",
  "'while' missing '('",
  "'while' missing ')'",
  "'do - while' missing '('",
  "'do' missing 'while'",
  "'do - while' missing ')'",
  "'switch' missing '('",
  "'switch' missing ')'",
  "'synchronized' missing '('",
  "'synchronized' missing ')'",
  "'case' missing ':'",
  "'default' missing ':'",
  "statement missing ';'",
  "Declaration expected",
  "Declaration missing ']'",
  "Declaration list missing ','",
  "'for' missing '('",
  "'for' missing ';'",
  "'for' missing ')'",
  "'throw' missing ';'",
  "'catch' missing '('",
  "missing 'catch' parameter",
  "too many 'catch' parameters",
  "'try' missing handlers",
  "Compound statement missing '}'",
  "Possibly missing '}'",
  "Statement expected",
  "Parameter type expected",
  "Parameter name expected",
  "Parameter missing ']'",
  "Parameter list missing ','",
  "Identifier expected",
  "';' expected",

  "'switch' missing '{'",
  "'synchronized' missing '{'",
  "'try' missing '{'",
  "'catch' missing '{'",
  "'finally' missing '{'",

  "Class declaration expected",
  "Class declaration missing '}'",
  "Missing ','",
  "Bad method declaration",
  "Method missing return type",
  "Method missing body",
  "Constructor missing body",
  "Method cant have body",
  "Duplicate modifier",
};

const char** strErrorMessage = strErrorMessageEnglish;

void  ErrorHandler::error(const LinCol& lc, error_t err)
{
  os << "$ERROR " << lc << strErrorMessage[err] << endl;
  nerrors++;
  PARSER_ASSERT(nerrors <= 7);
}
