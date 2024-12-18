%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern FILE *yyin;
extern int yylex(void);
extern int yylineno; // Linha atual do analisador léxico
extern char* yytext; // Texto atual do analisador léxico
void yyerror(const char *s);
%}

/* Declaração dos tokens retornados pelo scanner */
%start program
%token LET IN END INTEGER SKIP READ WRITE IF THEN ELSE FI WHILE DO
%token IDENTIFIER NUMBER
%token EQ LT GT ADD SUB MUL DIV EXP ASSIGN
%left ADD SUB
%left MUL DIV
%right EXP
%nonassoc EQ LT GT
%left '(' ')'

%%

/* Regras da gramática */

program:
    LET declarations IN command_sequence END                        {printf("Parsing completo: programa reconhecido.\n");}
;

declarations:
    INTEGER id_seq '.'                                              {printf("Declaracoes reconhecidas.\n");}
;

id_seq:
    IDENTIFIER
    | id_seq ',' IDENTIFIER                                         {printf("Identificador reconhecido.\n");}
;

command_sequence:
    command                      
    | command_sequence command                                      {printf("Comando unico ou sequencia reconhecidos.\n");}
;

command:
    SKIP ';'                                                        {printf("Comando SKIP reconhecido.\n");}
    | IDENTIFIER ASSIGN exp ';'                                     {printf("Comando ASSIGN reconhecido.\n");}
    | IF exp THEN command_sequence ELSE command_sequence FI ';'     {printf("Comando IF reconhecido.\n");}
    | WHILE exp DO command_sequence END ';'                         {printf("Comando WHILE reconhecido.\n");}
    | READ IDENTIFIER ';'                                           {printf("Comando READ reconhecido.\n");}
    | WRITE exp ';'                                                 {printf("Comando WRITE reconhecido.\n");}
;

exp:
    NUMBER      
    | IDENTIFIER 
    | '(' exp ')'
    | exp ADD exp
    | exp EQ exp
    | exp SUB exp
    | exp MUL exp
    | exp DIV exp
    | exp EXP exp
    | exp LT exp
    | exp GT exp
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s na linha %d, proximo a '%s'\n", s, yylineno, yytext);
}

int main(int argc, char **argv) {
    int opt;
    FILE *file = NULL;

    // Processa as opções de linha de comando
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                file = fopen(optarg, "r");
                if (!file) {
                    perror(optarg);
                    return 1;
                }
                yyin = file; // Redireciona a entrada do analisador léxico para o arquivo
                break;
            default:
                fprintf(stderr, "Uso: %s [-f <arquivo de entrada>]\n", argv[0]);
                return 1;
        }
    }

    if (file == NULL) {
        printf("Digite o codigo fonte para analise:\n");
        yyin = stdin; // Redireciona a entrada do analisador léxico para o terminal
    }

    yyparse();

    if (file != NULL) {
        fclose(file);
    }

    return 0;
}