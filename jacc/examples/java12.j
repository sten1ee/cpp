/* Java 1.2 parser for CUP.
 * Copyright (C) 1998 C. Scott Ananian <cananian@alumni.princeton.edu>
 * This program is released under the terms of the GPL; see the file
 * COPYING for more details.  There is NO WARRANTY on this code.
 */

/*
JDK 1.2 Features added:
  strictfp modifier.
  explicit_constructor_invocation ::= ...
        | primary DOT THIS LPAREN argument_list_opt RPAREN SEMICOLON ;
  field_access ::= ...
        |       name DOT SUPER DOT IDENTIFIER ;
  method_invocation ::= ...
        |       name DOT SUPER DOT IDENTIFIER LPAREN argument_list_opt RPAREN ;
*/
/*
parser code  {:
  Lexer lexer;

  public Grm(Lexer l) {
    this();
    lexer=l;
  }

  public void syntax_error(java_cup.runtime.Symbol current) {
    report_error("Syntax error (" + current.sym + ")", current);
  }
  public void report_error(String message, java_cup.runtime.Symbol info) {
    lexer.errorMsg(message, info);
  }
:};

scan with {: return lexer.nextToken(); :};
*/
%%PARSER_CODE {
#include <stdio.h>

extern intptr_t yylex();
extern int src_lineno;

extern "C" int yywrap() { return 1; }

class java12_lexer : public scanner
{
  public:
    lr_symbol* next_token()
    {
      return (lr_symbol*)yylex();
    }
};

int main(/*int argc, char* argv[]*/)
{
  java12_lexer	lexer;
  java12_parser parser(&lexer);

  int res;
  try {
    parser.parse();
    printf("%d line(s) successfully parsed", src_lineno);
    res = src_lineno;
  }
  catch (lr_parser::xfatal& exn) {
    printf("line %d: Syntax error", src_lineno);
    res = -src_lineno;
  }

  return res;
}
} //PARSER_CODE

%token BOOLEAN; // primitive_type
%token BYTE, SHORT, INT, LONG, CHAR; // integral_type
%token FLOAT, DOUBLE; // floating_point_type
%token LBRACK, RBRACK; // array_type
%token IDENTIFIER; // name
%token DOT; // qualified_name
%token SEMICOLON, MULT, COMMA, LBRACE, RBRACE, EQ, LPAREN, RPAREN, COLON;
%token PACKAGE; // package_declaration
%token IMPORT; // import_declaration
%token PUBLIC, PROTECTED, PRIVATE; // modifier
%token STATIC; // modifier
%token ABSTRACT, FINAL, NATIVE, SYNCHRONIZED, TRANSIENT, VOLATILE;
%token CLASS; // class_declaration
%token EXTENDS; // super
%token IMPLEMENTS; // interfaces
%token VOID; // method_header
%token THROWS; // throws
%token THIS, SUPER; // explicit_constructor_invocation
%token INTERFACE; // interface_declaration
%token IF, ELSE; // if_then_statement, if_then_else_statement
%token SWITCH; // switch_statement
%token CASE, DEFAULT; // switch_label
%token DO, WHILE; // while_statement, do_statement
%token FOR; // for_statement
%token BREAK; // break_statement
%token CONTINUE; // continue_statement
%token RETURN; // return_statement
%token THROW; // throw_statement
%token TRY; // try_statement
%token CATCH; // catch_clause
%token FINALLY; // finally
%token NEW; // class_instance_creation_expression
%token PLUSPLUS; // postincrement_expression
%token MINUSMINUS; // postdecrement_expression
%token PLUS, MINUS, COMP, NOT, DIV, MOD;
%token LSHIFT, RSHIFT, URSHIFT; // shift_expression
%token LT, GT, LTEQ, GTEQ, INSTANCEOF; // relational_expression
%token EQEQ, NOTEQ; // equality_expression
%token AND; // and_expression
%token XOR; // exclusive_or_expression
%token OR;  // inclusive_or_expression
%token ANDAND; // conditional_and_expression
%token OROR; // conditional_or_expression
%token QUESTION; // conditional_expression
%token MULTEQ, DIVEQ, MODEQ, PLUSEQ, MINUSEQ; // assignment_operator
%token LSHIFTEQ, RSHIFTEQ, URSHIFTEQ; // assignment_operator
%token ANDEQ, XOREQ, OREQ; // assignment_operator

%token <int>    INTEGER_LITERAL;
%token <double> FLOATING_POINT_LITERAL;
%token <bool>   BOOLEAN_LITERAL;
%token <char>   CHARACTER_LITERAL;
%token <char*>  STRING_LITERAL;
%token NULL_LITERAL;

// strictfp keyword, new in Java 1.2
%token STRICTFP;

// Reserved but unused:
%token CONST, GOTO;
/******************************************************************************
// 19.2) The Syntactic Grammar
%type goal;
// 19.3) Lexical Structure
%type literal;
// 19.4) Types, Values, and Variables
%type type, primitive_type, numeric_type;
%type integral_type, floating_point_type;
%type reference_type;
%type class_or_interface_type;
%type class_type, interface_type;
%type array_type;
// 19.5) Names
%type name, simple_name, qualified_name;
// 19.6) Packages
%type compilation_unit;
%type package_declaration_opt, package_declaration;
%type import_declarations_opt, import_declarations;
%type type_declarations_opt, type_declarations;
%type import_declaration;
%type single_type_import_declaration;
%type type_import_on_demand_declaration;
%type type_declaration;
// 19.7) Productions used only in the LALR(1) grammar
%type modifiers_opt, modifiers, modifier;
// 19.8.1) Class Declaration
%type class_declaration, super, super_opt;
%type interfaces, interfaces_opt, interface_type_list;
%type class_body;
%type class_body_declarations, class_body_declarations_opt;
%type class_body_declaration, class_member_declaration;
// 19.8.2) Field Declarations
%type field_declaration, variable_declarators, variable_declarator;
%type variable_declarator_id, variable_initializer;
// 19.8.3) Method Declarations
%type method_declaration, method_header, method_declarator;
%type formal_parameter_list_opt, formal_parameter_list;
%type formal_parameter;
%type throws_opt, throws;
%type class_type_list, method_body;
// 19.8.4) Static Initializers
%type static_initializer;
// 19.8.5) Constructor Declarations
%type constructor_declaration, constructor_declarator;
%type constructor_body;
%type explicit_constructor_invocation;
// 19.9.1) Interface Declarations
%type interface_declaration;
%type extends_interfaces_opt, extends_interfaces;
%type interface_body;
%type interface_member_declarations_opt, interface_member_declarations;
%type interface_member_declaration, constant_declaration;
%type abstract_method_declaration;
// 19.10) Arrays
%type array_initializer;
%type variable_initializers;
// 19.11) Blocks and Statements
%type block;
%type block_statements_opt, block_statements, block_statement;
%type local_variable_declaration_statement, local_variable_declaration;
%type statement, statement_no_short_if;
%type statement_without_trailing_substatement;
%type empty_statement;
%type labeled_statement, labeled_statement_no_short_if;
%type expression_statement, statement_expression;
%type if_then_statement;
%type if_then_else_statement, if_then_else_statement_no_short_if;
%type switch_statement, switch_block;
%type switch_block_statement_groups;
%type switch_block_statement_group;
%type switch_labels, switch_label;
%type while_statement, while_statement_no_short_if;
%type do_statement;
%type for_statement, for_statement_no_short_if;
%type for_init_opt, for_init;
%type for_update_opt, for_update;
%type statement_expression_list;
%type identifier_opt;
%type break_statement, continue_statement;
%type return_statement, throw_statement;
%type synchronized_statement, try_statement;
%type catches_opt, catches, catch_clause;
%type finally;
// 19.12) Expressions
%type primary, primary_no_new_array;
%type class_instance_creation_expression;
%type argument_list_opt, argument_list;
%type array_creation_expression;
%type dim_exprs, dim_expr, dims_opt, dims;
%type field_access, method_invocation, array_access;
%type postfix_expression;
%type postincrement_expression, postdecrement_expression;
%type unary_expression, unary_expression_not_plus_minus;
%type preincrement_expression, predecrement_expression;
%type cast_expression;
%type multiplicative_expression, additive_expression;
%type shift_expression, relational_expression, equality_expression;
%type and_expression, exclusive_or_expression, inclusive_or_expression;
%type conditional_and_expression, conditional_or_expression;
%type conditional_expression, assignment_expression;
%type assignment;
%type left_hand_side;
%type assignment_operator;
%type expression_opt, expression;
%type constant_expression;
******************************************************************************/
//start with goal;

// 19.2) The Syntactic Grammar
goal ::=	compilation_unit
	;

// 19.3) Lexical Structure.
literal ::=	INTEGER_LITERAL
	|	FLOATING_POINT_LITERAL
	|	BOOLEAN_LITERAL
	|	CHARACTER_LITERAL
	|	STRING_LITERAL
	|	NULL_LITERAL
	;

// 19.4) Types, Values, and Variables
type	::=	primitive_type
	|	reference_type
	;
primitive_type ::=
		numeric_type
	|	BOOLEAN
	;
numeric_type::=	integral_type
	|	floating_point_type
	;
integral_type ::= 
		BYTE 
	|	SHORT 
	|	INT 
	|	LONG 
	|	CHAR 
	;
floating_point_type ::= 
		FLOAT 
	|	DOUBLE
	;

reference_type ::=
		class_or_interface_type
	|	array_type
	;
class_or_interface_type ::= name;

class_type ::=	class_or_interface_type;
interface_type ::= class_or_interface_type;		

array_type ::=	primitive_type dims
	|	name dims
	;

// 19.5) Names
name	::=	simple_name
	|	qualified_name
	;
simple_name ::=	IDENTIFIER
	;
qualified_name ::=
		name DOT IDENTIFIER
	;

// 19.6) Packages
compilation_unit ::=
		package_declaration_opt 
		import_declarations_opt
		type_declarations_opt
		;
package_declaration_opt ::= package_declaration | ;
import_declarations_opt ::= import_declarations | ;
type_declarations_opt   ::= type_declarations   | ;

import_declarations ::= 
		import_declaration
	|	import_declarations import_declaration
	;
type_declarations ::= 
		type_declaration
	|	type_declarations type_declaration
	;
package_declaration ::= 
		PACKAGE name SEMICOLON
	;
import_declaration ::= 
		single_type_import_declaration
	|	type_import_on_demand_declaration
	;
single_type_import_declaration ::=
		IMPORT name SEMICOLON
	;
type_import_on_demand_declaration ::=
		IMPORT name DOT MULT SEMICOLON
	;
type_declaration ::=
		class_declaration
	|	interface_declaration
	|	SEMICOLON
	;

// 19.7) Productions used only in the LALR(1) grammar
modifiers_opt::=
	|	modifiers
	;
modifiers ::= 	modifier
	|	modifiers modifier
	;
modifier ::=	PUBLIC | PROTECTED | PRIVATE
	|	STATIC
	|	ABSTRACT | FINAL | NATIVE | SYNCHRONIZED | TRANSIENT | VOLATILE
	|	STRICTFP // note that semantic analysis must check that the
	                 // context of the modifier allows strictfp.
	;

// 19.8) Classes

// 19.8.1) Class Declaration:
class_declaration ::= 
	modifiers_opt CLASS IDENTIFIER super_opt interfaces_opt class_body
	;
super ::=	EXTENDS class_type
	;
super_opt ::=	
	|	super
	;
interfaces ::=	IMPLEMENTS interface_type_list
	;
interfaces_opt::=
	|	interfaces 
	;
interface_type_list ::=
		interface_type
	|	interface_type_list COMMA interface_type
	;
class_body ::=	LBRACE class_body_declarations_opt RBRACE 
	;
class_body_declarations_opt ::= 
	|	class_body_declarations ;
class_body_declarations ::= 
		class_body_declaration
	|	class_body_declarations class_body_declaration
	;
class_body_declaration ::=
		class_member_declaration
	|	static_initializer
	|	constructor_declaration
	|	block
	;
class_member_declaration ::=
		field_declaration
	|	method_declaration
	/* repeat the prod for 'class_declaration' here: */
	|	modifiers_opt CLASS IDENTIFIER super_opt interfaces_opt class_body
	|	interface_declaration
	;

// 19.8.2) Field Declarations
field_declaration ::= 
		modifiers_opt type variable_declarators SEMICOLON
	;
variable_declarators ::=
		variable_declarator
	|	variable_declarators COMMA variable_declarator
	;
variable_declarator ::=
		variable_declarator_id
	|	variable_declarator_id EQ variable_initializer
	;
variable_declarator_id ::=
		IDENTIFIER
	|	variable_declarator_id LBRACK RBRACK
	;
variable_initializer ::=
		expression
	|	array_initializer
	;

// 19.8.3) Method Declarations
method_declaration ::=
		method_header method_body
	;
method_header ::=
		modifiers_opt type method_declarator throws_opt
	|	modifiers_opt VOID method_declarator throws_opt
	;
method_declarator ::=
		IDENTIFIER LPAREN formal_parameter_list_opt RPAREN
	|	method_declarator LBRACK RBRACK // deprecated
	// be careful; the above production also allows 'void foo() []'
	;
formal_parameter_list_opt ::=
	|	formal_parameter_list
	;
formal_parameter_list ::=
		formal_parameter
	|	formal_parameter_list COMMA formal_parameter
	;
formal_parameter ::=
		type variable_declarator_id
	|	FINAL type variable_declarator_id
	;
throws_opt ::=	
	|	throws
	;
throws ::=	THROWS class_type_list
	;
class_type_list ::=
		class_type
	|	class_type_list COMMA class_type
	;
method_body ::=	block
	|	SEMICOLON
	;

// 19.8.4) Static Initializers
static_initializer ::=
		STATIC block
	;

// 19.8.5) Constructor Declarations
constructor_declaration ::=
		modifiers_opt constructor_declarator throws_opt 
			constructor_body
	;
constructor_declarator ::=
		simple_name LPAREN formal_parameter_list_opt RPAREN
	;
constructor_body ::=
		LBRACE explicit_constructor_invocation
			block_statements RBRACE
	|	LBRACE explicit_constructor_invocation RBRACE
	|	LBRACE block_statements RBRACE
	|	LBRACE RBRACE
	;
explicit_constructor_invocation ::=
		THIS LPAREN argument_list_opt RPAREN SEMICOLON
	|	SUPER LPAREN argument_list_opt RPAREN SEMICOLON
	|	primary DOT THIS LPAREN argument_list_opt RPAREN SEMICOLON
	|	primary DOT SUPER LPAREN argument_list_opt RPAREN SEMICOLON
	;

// 19.9) Interfaces

// 19.9.1) Interface Declarations
interface_declaration ::=
		modifiers_opt INTERFACE IDENTIFIER extends_interfaces_opt 
			interface_body
	;
extends_interfaces_opt ::=
	|	extends_interfaces
	;
extends_interfaces ::=
		EXTENDS interface_type
	|	extends_interfaces COMMA interface_type
	;
interface_body ::=
		LBRACE interface_member_declarations_opt RBRACE
	;
interface_member_declarations_opt ::=
	|	interface_member_declarations
	;
interface_member_declarations ::=
		interface_member_declaration
	|	interface_member_declarations interface_member_declaration
	;
interface_member_declaration ::=
		constant_declaration
	|	abstract_method_declaration
	|	class_declaration
	|	interface_declaration
	;
constant_declaration ::=
		field_declaration
	// need to semantically check that modifiers of field declaration
	// include only PUBIC, STATIC, or FINAL.  Other modifiers are
	// disallowed.
	;
abstract_method_declaration ::=
		method_header SEMICOLON
	;

// 19.10) Arrays
array_initializer ::=
		LBRACE variable_initializers COMMA RBRACE
	|	LBRACE variable_initializers RBRACE
	|	LBRACE COMMA RBRACE
	|	LBRACE RBRACE
	;
variable_initializers ::=
		variable_initializer
	|	variable_initializers COMMA variable_initializer
	;

// 19.11) Blocks and Statements
block ::=	LBRACE block_statements_opt RBRACE
	;
block_statements_opt ::=
	|	block_statements
	;
block_statements ::=
		block_statement
	|	block_statements block_statement
	;
block_statement ::=
		local_variable_declaration_statement
	|	statement
	|	class_declaration
	|	interface_declaration
	;
local_variable_declaration_statement ::=
		local_variable_declaration SEMICOLON
	;
local_variable_declaration ::=
		type variable_declarators
	|	FINAL type variable_declarators
	;
statement ::=	statement_without_trailing_substatement
	|	labeled_statement
	|	if_then_statement
	|	if_then_else_statement
	|	while_statement
	|	for_statement
	;
statement_no_short_if ::=
		statement_without_trailing_substatement
	|	labeled_statement_no_short_if
	|	if_then_else_statement_no_short_if
	|	while_statement_no_short_if
	|	for_statement_no_short_if
	;
statement_without_trailing_substatement ::=
		block
	|	empty_statement
	|	expression_statement
	|	switch_statement
	|	do_statement
	|	break_statement
	|	continue_statement
	|	return_statement
	|	synchronized_statement
	|	throw_statement
	|	try_statement
	;
empty_statement ::=
		SEMICOLON
	;
labeled_statement ::=
		IDENTIFIER COLON statement
	;
labeled_statement_no_short_if ::=
		IDENTIFIER COLON statement_no_short_if
	;
expression_statement ::=
		statement_expression SEMICOLON
	;
statement_expression ::=
		assignment
	|	preincrement_expression
	|	predecrement_expression
	|	postincrement_expression
	|	postdecrement_expression
	|	method_invocation
	|	class_instance_creation_expression
	;
if_then_statement ::=
		IF LPAREN expression RPAREN statement
	;
if_then_else_statement ::=
		IF LPAREN expression RPAREN statement_no_short_if 
			ELSE statement
	;
if_then_else_statement_no_short_if ::=
		IF LPAREN expression RPAREN statement_no_short_if
			ELSE statement_no_short_if
	;
switch_statement ::=
		SWITCH LPAREN expression RPAREN switch_block
	;
switch_block ::=
		LBRACE switch_block_statement_groups switch_labels RBRACE
	|	LBRACE switch_block_statement_groups RBRACE
	|	LBRACE switch_labels RBRACE
	|	LBRACE RBRACE
	;
switch_block_statement_groups ::=
		switch_block_statement_group
	|	switch_block_statement_groups switch_block_statement_group
	;
switch_block_statement_group ::=
		switch_labels block_statements
	;
switch_labels ::=
		switch_label
	|	switch_labels switch_label
	;
switch_label ::=
		CASE constant_expression COLON
	|	DEFAULT COLON
	;

while_statement ::=
		WHILE LPAREN expression RPAREN statement
	;
while_statement_no_short_if ::=
		WHILE LPAREN expression RPAREN statement_no_short_if
	;
do_statement ::=
		DO statement WHILE LPAREN expression RPAREN SEMICOLON
	;
for_statement ::=
		FOR LPAREN for_init_opt SEMICOLON expression_opt SEMICOLON
			for_update_opt RPAREN statement
	;
for_statement_no_short_if ::=
		FOR LPAREN for_init_opt SEMICOLON expression_opt SEMICOLON
			for_update_opt RPAREN statement_no_short_if
	;
for_init_opt ::=
	|	for_init
	;
for_init ::=	statement_expression_list
	|	local_variable_declaration
	;
for_update_opt ::=
	|	for_update
	;
for_update ::=	statement_expression_list
	;
statement_expression_list ::=
		statement_expression
	|	statement_expression_list COMMA statement_expression
	;

identifier_opt ::= 
	|	IDENTIFIER
	;

break_statement ::=
		BREAK identifier_opt SEMICOLON
	;

continue_statement ::=
		CONTINUE identifier_opt SEMICOLON
	;
return_statement ::=
		RETURN expression_opt SEMICOLON
	;
throw_statement ::=
		THROW expression SEMICOLON
	;
synchronized_statement ::=
		SYNCHRONIZED LPAREN expression RPAREN block
	;
try_statement ::=
		TRY block catches
	|	TRY block catches_opt finally
	;
catches_opt ::=
	|	catches
	;
catches ::=	catch_clause
	|	catches catch_clause
	;
catch_clause ::=
		CATCH LPAREN formal_parameter RPAREN block
	;
finally ::=	FINALLY block
	;

// 19.12) Expressions
primary ::=	primary_no_new_array
	|	array_creation_expression
	;
primary_no_new_array ::=
		literal
	|	THIS
	|	LPAREN expression RPAREN
	|	class_instance_creation_expression
	|	field_access
	|	method_invocation
	|	array_access
	|	primitive_type DOT CLASS
	|	VOID DOT CLASS
	|	array_type DOT CLASS
	|	name DOT CLASS
	|	name DOT THIS
	;
class_instance_creation_expression ::=
		NEW class_type LPAREN argument_list_opt RPAREN
	|	NEW class_type LPAREN argument_list_opt RPAREN class_body
	|	primary DOT NEW IDENTIFIER
			LPAREN argument_list_opt RPAREN
	|	primary DOT NEW IDENTIFIER
			LPAREN argument_list_opt RPAREN class_body
	;
argument_list_opt ::=
	|	argument_list
	;
argument_list ::=
		expression
	|	argument_list COMMA expression
	;
array_creation_expression ::=
		NEW primitive_type dim_exprs dims_opt
	|	NEW class_or_interface_type dim_exprs dims_opt
	|	NEW primitive_type dims array_initializer
	|	NEW class_or_interface_type dims array_initializer
	;
dim_exprs ::=	dim_expr
	|	dim_exprs dim_expr
	;
dim_expr ::=	LBRACK expression RBRACK
	;
dims_opt ::=
	|	dims
	;
dims ::=	LBRACK RBRACK
	|	dims LBRACK RBRACK
	;
field_access ::=
		primary DOT IDENTIFIER
	|	SUPER DOT IDENTIFIER
	|	name DOT SUPER DOT IDENTIFIER
	;
method_invocation ::=
		name LPAREN argument_list_opt RPAREN
	|	primary DOT IDENTIFIER LPAREN argument_list_opt RPAREN
	|	SUPER DOT IDENTIFIER LPAREN argument_list_opt RPAREN
	|	name DOT SUPER DOT IDENTIFIER LPAREN argument_list_opt RPAREN
	;
array_access ::=
		name LBRACK expression RBRACK
	|	primary_no_new_array LBRACK expression RBRACK
	;
postfix_expression ::=
		primary
	|	name
	|	postincrement_expression
	|	postdecrement_expression
	;
postincrement_expression ::=
		postfix_expression PLUSPLUS
	;
postdecrement_expression ::=
		postfix_expression MINUSMINUS
	;
unary_expression ::=
		preincrement_expression
	|	predecrement_expression
	|	PLUS unary_expression
	|	MINUS unary_expression
	|	unary_expression_not_plus_minus
	;
preincrement_expression ::=
		PLUSPLUS unary_expression
	;
predecrement_expression ::=
		MINUSMINUS unary_expression
	;
unary_expression_not_plus_minus ::=
		postfix_expression
	|	COMP unary_expression
	|	NOT unary_expression
	|	cast_expression
	;
cast_expression ::=
		LPAREN primitive_type dims_opt RPAREN unary_expression
	|	LPAREN expression RPAREN unary_expression_not_plus_minus
	|	LPAREN name dims RPAREN unary_expression_not_plus_minus
	;
multiplicative_expression ::=
		unary_expression
	|	multiplicative_expression MULT unary_expression
	|	multiplicative_expression DIV unary_expression
	|	multiplicative_expression MOD unary_expression
	;
additive_expression ::=
		multiplicative_expression
	|	additive_expression PLUS multiplicative_expression
	|	additive_expression MINUS multiplicative_expression
	;
shift_expression ::=
		additive_expression
	|	shift_expression LSHIFT additive_expression
	|	shift_expression RSHIFT additive_expression
	|	shift_expression URSHIFT additive_expression
	;
relational_expression ::=
		shift_expression
	|	relational_expression LT shift_expression
	|	relational_expression GT shift_expression
	|	relational_expression LTEQ shift_expression
	|	relational_expression GTEQ shift_expression
	|	relational_expression INSTANCEOF reference_type
	;
equality_expression ::=
		relational_expression
	|	equality_expression EQEQ relational_expression
	|	equality_expression NOTEQ relational_expression
	;
and_expression ::=
		equality_expression
	|	and_expression AND equality_expression
	;
exclusive_or_expression ::=
		and_expression
	|	exclusive_or_expression XOR and_expression
	;
inclusive_or_expression ::=
		exclusive_or_expression
	|	inclusive_or_expression OR exclusive_or_expression
	;
conditional_and_expression ::=
		inclusive_or_expression
	|	conditional_and_expression ANDAND inclusive_or_expression
	;
conditional_or_expression ::=
		conditional_and_expression
	|	conditional_or_expression OROR conditional_and_expression
	;
conditional_expression ::=
		conditional_or_expression
	|	conditional_or_expression QUESTION expression 
			COLON conditional_expression
	;
assignment_expression ::=
		conditional_expression
	|	assignment
	;
assignment ::=	left_hand_side assignment_operator assignment_expression
	;
left_hand_side ::=
		name
	|	field_access
	|	array_access
	;
assignment_operator ::=
		EQ
	|	MULTEQ
	|	DIVEQ
	|	MODEQ
	|	PLUSEQ
	|	MINUSEQ
	|	LSHIFTEQ
	|	RSHIFTEQ
	|	URSHIFTEQ
	|	ANDEQ
	|	XOREQ
	|	OREQ
	;
expression_opt ::=
	|	expression
	;
expression ::=	assignment_expression
	;
constant_expression ::=
		expression
	;
