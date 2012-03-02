%{
#include <stdio.h>
#include <string.h>
#include "tiny_sql.hh"
extern YYSTYPE yylval;
%}
%%
CREATE printf("CREATE\n"); return CREATE;
TABLE printf("TABLE\n"); return TABLE;
INT printf("INT\n"); yylval.number=0; return INTTOK;
STR20 printf("STR20\n"); yylval.number=1; return STR20TOK;
\( printf("OPAREN\n"); return OPAREN;
\) printf("EPAREN\n"); return EPAREN;
,  printf("COLON\n"); return COLON;
[a-zA-Z][a-zA-Z0-9]*    yylval.str=strdup(yytext); printf("NAME\n"); return NAME;
\n                      printf("NEWLINE\n"); return NEWLINE;
[ \t]+                  /* ignore whitespace */;
%%
