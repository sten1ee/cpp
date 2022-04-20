%% PARSER_CPP_INCLUDES {#include <stdio.h>}
%token a, b;
%type int_sym<int> S;

AB
    : S(s)      {printf("Result is: %d\n", s)}
    ;
S
    : a S(s) b  {result = s + 1}
    |           {result = 0}
    ;
