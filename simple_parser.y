%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

extern FILE *yyin;
extern int yylex(void);
extern int yylineno; // Linha atual do analisador léxico
extern char* yytext; // Texto atual do analisador léxico
void yyerror(const char *s);

typedef struct Id_Node Id_Node;
struct Id_Node 
{
	char *id;
	Id_Node *nxt;
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
	return new_id;
};

typedef struct Context Context;
struct Context 
{
	Id_Node *id_table;  // Tabela de identificadores
	int errors;         // Numero de erros
};

Context global_context;

void create_context() 
{
	global_context.id_table = NULL;	
	global_context.errors = 0;	
};

void add_new_indentifier(char *id) 
{
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
	if (id_node_find(global_context.id_table, id) == NULL) {
		fprintf(stderr, "ERRO: Variavel `%s` nao foi declarada\n", id);
		global_context.errors++;
	}
}

%}

%union semrec // Valores semanticos
{
	int val;
	char *id;
}

/* Declaração dos tokens retornados pelo scanner */
%start program
%token LET IN END INTEGER SKIP READ WRITE IF THEN ELSE FI WHILE DO
%token <id>  IDENTIFIER 
%token <val> NUMBER
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
    SKIP ';'                                                        {}
    | IDENTIFIER ASSIGN exp ';'                                     { check_identifier($1); }
    | IF exp THEN command_sequence ELSE command_sequence FI ';'     {}
    | WHILE exp DO command_sequence END ';'                         {}
    | READ IDENTIFIER ';'                                           { check_identifier($2); }
    | WRITE exp ';'                                                 {}
;

exp:
    NUMBER      
    | IDENTIFIER                                                    { check_identifier($1); }
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

	if (global_context.errors > 0) {
		fprintf(stdout, "Compilacao terminada com %d erros\n", global_context.errors);
		return 1;
	}
	fprintf(stdout, "Compilacao terminada com sucesso\n");

    return 0;
}
