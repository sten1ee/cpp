#define __USELOCALES__
#include <locale.h>
#include <stdio.h>

static int  ql = printf("*** locale in use: %s", setlocale(LC_CTYPE, ""));
