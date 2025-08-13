%{
#include "y.tab.h"
%}
%%

"<<"	{ return LARROW; }
">>"	{ return RARROW; }
"int"	{ return INT: }
[a-zA-Z]+	{ yylval.str = strdup(yytext); return ID; }
[0-9]:

