%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "code_generator.h"

extern FILE *yyin;
extern int yylex(void);
extern int yylineno; // Linha atual do analisador léxico
extern char* yytext; // Texto atual do analisador léxico
void yyerror(const char *s);
void check_identifier(char *id);
int temp_count = 0;

typedef struct Id_Node Id_Node;
struct Id_Node 
{
	char *id;
	Id_Node *nxt;
	bool used;
};


Id_Node* id_node_find(Id_Node *node, char *id)  // Retorna ponteiro para id, senao existir retorna NULL
{ 
	while(node != NULL) {
		if (strcmp(node->id, id) == 0) break;
		node = node->nxt;
	}
	return node;
};

Id_Node *id_node_append(Id_Node *node, char *id) // Cria novo e retorna novo node
{
	Id_Node *new_id = malloc(sizeof(*node));
	new_id->id = id;
	new_id->nxt = node;
	new_id->used = false;
	return new_id;
};

typedef struct Context Context;
struct Context 
{
	Id_Node *id_table;  // Tabela de identificadores
	int errors;         // Numero de erros
	int warnings;
};

Context global_context;

void create_context() 
{
	global_context.id_table = NULL;	
	global_context.errors = 0;	
	global_context.warnings = 0;	
};

void add_new_indentifier(char *id) 
{
	printf("ID: %s\n",id);
	if (id_node_find(global_context.id_table, id) != NULL) {
		fprintf(stderr, "ERRO: Variavel `%s` ja foi declarada, nome repetido\n", id);
		global_context.errors++;
	}
	else {
		global_context.id_table = id_node_append(global_context.id_table, id);
	}
}

void check_identifier(char *id)
{
	Id_Node* node = id_node_find(global_context.id_table, id);
	if (node == NULL) {
		fprintf(stderr, "ERRO: Variavel `%s` nao foi declarada\n", id);
		global_context.errors++;
	}
	else {
		node->used = true;
	}
}

void check_unused_variables() {
	Id_Node *node = global_context.id_table;
	while(node != NULL) {
		if (!node->used) {
            fprintf(stderr, "WARNING: Variavel `%s` nao foi utilizada\n", node->id);
			global_context.warnings += 1;
		}
		node = node->nxt;
	}
}

%}

//%union semrec // Valores semanticos
// {
// 	int val;
// 	char *id;
// }

%union {
    int ival;
    char *sval;
}

/* Declaração dos tokens e suas associações de tipo */
%token <sval> IDENTIFIER
%token <ival> NUMBER

/* Declaração dos tipos de dados para as não-terminais */
%type <ival> exp
%type <sval> command

/* Declaração dos tokens retornados pelo scanner */
%start program
%token LET IN END INTEGER SKIP READ WRITE IF THEN ELSE FI WHILE DO
//%token <id>  IDENTIFIER 
//%token <val> NUMBER
%token EQ LT GT ADD SUB MUL DIV EXP ASSIGN
%left ADD SUB
%left MUL DIV
%right EXP
%nonassoc EQ LT GT
%left '(' ')'

%%

/* Regras da gramática */

program:
    LET declarations IN command_sequence END                        {}
;

declarations: /* empty */
    | INTEGER id_seq '.'                                            {}
;

id_seq:
    IDENTIFIER                                                      { add_new_indentifier($1); }
    | id_seq ',' IDENTIFIER                                         { add_new_indentifier($3); }
;

command_sequence:
    command                      
    | command_sequence command                                      {}
;

command:
      IDENTIFIER ASSIGN exp ';' { check_identifier($1); printf("ASSIGN %s t%d\n", $1, $3); }
    | IF exp THEN command_sequence FI ';' { printf("IF t%d ... THEN ... FI\n", $2); }
    | WHILE exp DO command_sequence END ';' { printf("WHILE t%d ... DO ...\n", $2); }
    | READ IDENTIFIER ';' { printf("READ %s\n", $2); }
    | WRITE exp ';' { printf("WRITE t%d\n", $2); }
    ;

exp:
      NUMBER { $$ = temp_count++; printf("t%d = %d\n", $$, $1);  codeGenerator(OP_LD_INT, $1); }
    | IDENTIFIER { check_identifier($1); $$ = temp_count++; printf("t%d = %s\n", $$, $1); codeGenerator(OP_LD_VAR, temp_count);}
    | '(' exp ')' { $$ = $2; }
    | exp ADD exp { $$ = temp_count++; printf("t%d = t%d + t%d\n", $$, $1, $3); codeGenerator(OP_ADD, 0);}
    | exp SUB exp { $$ = temp_count++; printf("t%d = t%d - t%d\n", $$, $1, $3); codeGenerator(OP_SUB, 0);}
    | exp MUL exp { $$ = temp_count++; printf("t%d = t%d * t%d\n", $$, $1, $3); codeGenerator(OP_MUL, 0);}
    | exp DIV exp { $$ = temp_count++; printf("t%d = t%d / t%d\n", $$, $1, $3); codeGenerator(OP_DIV, 0);}
    | exp EXP exp { $$ = temp_count++; printf("t%d = t%d ^ t%d\n", $$, $1, $3); codeGenerator(OP_EXP, 0);}
    | exp EQ exp { $$ = temp_count++; printf("t%d = (t%d == t%d)\n", $$, $1, $3); codeGenerator(OP_EQ, 0);}
    | exp LT exp { $$ = temp_count++; printf("t%d = (t%d < t%d)\n", $$, $1, $3); codeGenerator(OP_LT, 0);}
    | exp GT exp { $$ = temp_count++; printf("t%d = (t%d > t%d)\n", $$, $1, $3); codeGenerator(OP_GT, 0);}
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s na linha %d, proximo a '%s'\n", s, yylineno, yytext);
	global_context.errors++;
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

	create_context();
	
    yyparse();

    if (file != NULL) {
        fclose(file);
    }

	check_unused_variables();
    fetch_execute_cycle();

	if (global_context.warnings > 0) {
		fprintf(stdout, "Compilacao terminada com %d warnings\n", global_context.warnings);
	}

	if (global_context.errors > 0) {
		fprintf(stdout, "Compilacao terminada com %d erros\n", global_context.errors);
		return 1;
	}
	fprintf(stdout, "Compilacao terminada com sucesso\n");

    return 0;
}