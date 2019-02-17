%{
#include <stdio.h>
#include "ast.h"
#include "y.tab.h"
#include <string.h>
%}
%%
[0-9]+      { yylval.ival = atoi(yytext);
              return(CONSTANT); }
"else"         { return(ELSE); }
"if"         { return(IF); }
"int"         { return(INT); }
"return"      { return(RETURN); }
"list"      { return(LIST); }
"void"         { return(VOID); }
"while"         { return(WHILE); }
"<<"         { return(OUT); }
"<="         { return(LE_OP); }
">="         { return(GE_OP); }
"=="         { return(EQ_OP); }
"="         { return('='); }
"!="         { return(NE_OP); }
"!"         { return('!'); }
";"         { return(';'); }
("{"|"<%")      { return('{'); }
("}"|"%>")      { return('}'); }
","         { return(','); }
"("         { return('('); }
")"         { return(')'); }
("["|"<:")      { return('['); }
("]"|":>")      { return(']'); }
"-"         { return('-'); }
"+"         { return('+'); }
"*"         { return('*'); }
"/"         { return('/'); }
"<"         { return('<'); }
">"         { return('>'); }
"^"         { return('^'); }
[a-zA-Z_][a-zA-Z_0-9]*  {
              yylval.sval = strdup(yytext);
              return ID; }
[ \t\v\n\f]      { ; }
.           { printf("error near %s\n", yytext);
              exit(-1); }
%%
int yywrap() {
   return 1;
}
