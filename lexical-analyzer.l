%{
#include "simple_parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int yylineno = 1; 
%}

ID	[a-zA-Z][a-zA-Z0-9]*
DIGIT	[0-9]+

%%
do {return DO; }
else {return ELSE; }
end {return END; }
fi {return FI; }
if {return IF; }
in {return IN; }
integer {return INTEGER; }
let {return LET; }
read {return READ; }
skip {return SKIP; }
then {return THEN; }
while {return WHILE; }
write {return WRITE; }
{DIGIT}+ {return NUMBER; }
{ID} {return IDENTIFIER;}
":=" {return ASSIGN; }
"=" {return EQ; }
"<" {return LT; }
">" {return GT; }
"+" {return ADD; }
"-" {return SUB; }
"*" {return MUL; }
"/" {return DIV; }
"^" {return EXP; }
"(" { return '('; }
")" { return ')'; }
";" { return ';'; }
"," { return ','; }
"." { return '.'; }
\n { yylineno++; } { /* contar linhas */ }
[ \t\n\r]+ { /* ignorar quebra de linha, tabulação, espaços */ }
%%

int yywrap(){
  return 1;
};

