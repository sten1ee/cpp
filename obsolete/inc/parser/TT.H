#ifndef PARSER_TT_H
#define PARSER_TT_H

enum TT {
  TT_EXP          = 0x0100,           //token that can appear in expression
  TT_PREFIX       = 0x0200   | TT_EXP,//token that expression can start with
  TT_PRIME_TYPE   = 0x0400,
  TT_MODIF        = 0x0800,
  TT_CLASS_MODIF  = TT_MODIF | 0x0010,
  TT_METHOD_MODIF = TT_MODIF | 0x0020,
  TT_VAR_MODIF    = TT_MODIF | 0x0040,
  TT_LOCAL_MODIF  = TT_MODIF | 0x0080,
  TT_GLOBAL       = 0x1000,

  TT_NOTOKEN      = 0,// returned on error
  TT_EOF,

  TT_extends,
  TT_implements,
  TT_package,
  TT_import,
  TT_break,
  TT_case,
  TT_catch,
  TT_continue,
  TT_default,
  TT_do,
  TT_else,
  TT_finally,
  TT_for,
  TT_if,
  TT_return,
  TT_switch,
  TT_throw,
  TT_throws,
  TT_try,
  TT_while,

  TT_ClosFigBrek     ,// }
  TT_OpenFigBrek     ,// {
  TT_Semicol         ,// ;

  TT_class           = TT_GLOBAL,
  TT_interface       ,

  TT_void            = TT_PRIME_TYPE,
  TT_byte            ,
  TT_short           ,
  TT_int             ,
  TT_long            ,
  TT_float           ,
  TT_double          ,
  TT_boolean         ,
  TT_char            ,

  TT_Int             = TT_PREFIX,
  TT_Long            ,
  TT_Float           ,
  TT_Double          ,
  TT_Char            ,
  TT_String          ,
  TT_true            ,
  TT_false           ,
  TT_new             ,
  TT_null            ,
  TT_Symbol          ,
  TT_super           ,// in non-static methods only!
  TT_this            ,// in non-static methods only!
  TT_LogNot          ,// !
  TT_BitNot          ,// ~
  TT_Minus           ,// -
  TT_Decrement       ,// --
  TT_Plus            ,// +
  TT_Increment       ,// ++
  TT_OpenBrek        ,// (

  TT_ClosBrek        = TT_EXP,// )
  TT_instanceof      ,
  TT_NotEq           ,// !=
  TT_Mod             ,// %
  TT_ModAssign       ,// %=
  TT_BitAnd          ,// &
  TT_LogAnd          ,// &&
  TT_BitAndAssign    ,// &=
  TT_Mul             ,// *
  TT_MulAssign       ,// *=
  TT_PlusAssign      ,// +=
  TT_Comma           ,// ,
  TT_MinusAssign     ,// -=
  TT_Dot             ,// .
  TT_Div             ,// /
  TT_CComment        ,// /*
  TT_CPPComment      ,// //
  TT_DivAssign       ,// /=
  TT_DoubleDot       ,// :
  TT_Question        ,// ?
  TT_Less            ,// <
  TT_LeftShift       ,// <<
  TT_LeftShiftAssign ,// <<=
  TT_LessEq          ,// <=
  TT_Assign          ,// =
  TT_Eq              ,// ==
  TT_Greater         ,// >
  TT_GreaterEq       ,// >=
  TT_RightShift      ,// >>
  TT_RightShiftAssign,// >>=
  TT_RightLogShift   ,// >>>
  TT_RightLogShAssign,// >>=
  TT_OpenKvadBrek    ,// [
  TT_ClosKvadBrek    ,// ]
  TT_BitXor          ,// ^
  TT_BitXorAssign    ,// ^=
  TT_BitOr           ,// |
  TT_BitOrAssign     ,// |=
  TT_LogOr           ,// ||

  TT_public       = TT_CLASS_MODIF | TT_METHOD_MODIF | TT_VAR_MODIF + 1,
  TT_private      =                  TT_METHOD_MODIF | TT_VAR_MODIF + 2,
  TT_protected    =                  TT_METHOD_MODIF | TT_VAR_MODIF + 3,
  TT_static       =                  TT_METHOD_MODIF | TT_VAR_MODIF + 4,
  TT_final        = TT_CLASS_MODIF | TT_METHOD_MODIF | TT_VAR_MODIF + 5,
  TT_synchronized =                  TT_METHOD_MODIF                + 6,
  TT_threadsafe   =                                    TT_VAR_MODIF + 7,
  TT_transient    =                                    TT_VAR_MODIF + 8,
  TT_native       =                  TT_METHOD_MODIF                + 9,
  TT_abstract     =                  TT_METHOD_MODIF                +10
};

#endif //PARSER_TT_H
