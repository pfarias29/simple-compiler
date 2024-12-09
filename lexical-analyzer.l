%{
#include <stdio.h>
#include <string.h>
%}
ID	[a-zA-Z][a-zA-Z0-9]*
DIGIT	[0-9]
%%
":=" {fprintf(yyout, "ASSIGN\n"); }
"<" {fprintf(yyout, "LT\n"); }
">" {fprintf(yyout, "GT\n"); }
"+" {fprintf(yyout, "ADD\n"); }
"-" {fprintf(yyout, "SUB\n"); }
"*" {fprintf(yyout, "MUL\n"); }
"/" {fprintf(yyout, "DIV\n"); }
"^" {fprintf(yyout, "EXP\n"); }
 {DIGIT} {fprintf(yyout, "NUMBER, %s\n", yytext); }
 do {fprintf(yyout, "DO\n"); }
 else {fprintf(yyout, "ELSE\n"); }
 end {fprintf(yyout, "END\n"); }
 fi {fprintf(yyout, "FI\n"); }
 if {fprintf(yyout, "IF\n"); }
 in {fprintf(yyout, "IN\n"); }
 integer {fprintf(yyout, "INTEGER\n"); }
 let {fprintf(yyout, "LET\n"); }
 read {fprintf(yyout, "READ\n"); }
 skip {fprintf(yyout, "SKIP\n"); }
 then {fprintf(yyout, "THEN\n"); }
 while {fprintf(yyout, "WHILE\n"); }
 write {fprintf(yyout, "WRITE\n"); }
 {ID} {fprintf(yyout, "IDENTIFIER, %s\n", yytext); }
 [ \t\n]+
 ";"     {fprintf(yyout, "END_OF_EXPRESSION\n"); }
 "."     {fprintf(yyout, "END_OF_VARIABLE_INITIALIZATION\n"); }
 ","     {fprintf(yyout, "END_OF_VARIABLE_DECLARATION\n"); }
%%
/* recomendavel declarar sempre
   funcao yywrap() */
int yywrap(){};

/* programa principal */
int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Forneça um arquivo de entrada!");
    return 0;
  }

  extern FILE *yyin, * yyout;
  
  yyin = fopen(argv[1],"r");
  yyout = fopen("Output.txt", "w");

  yylex(); /* scanner gerado por Flex */
  
  return 0;
}
