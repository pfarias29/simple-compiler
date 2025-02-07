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
void check_identifier_context(enum code_ops operation, char *id);
int temp_count = 0;
int erros;

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
	Id_Node *new_id = malloc(sizeof(Id_Node));
    if (!new_id) {
        fprintf(stderr, "Erro: falha na alocação de memória\n");
        exit(EXIT_FAILURE);
    }
    new_id->id = strdup(id);
    if (!new_id->id) {
        fprintf(stderr, "Erro: falha na alocação de memória para o identificador\n");
        free(new_id);
        exit(EXIT_FAILURE);
    }
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

void check_identifier_context(enum code_ops operation, char *id)
{
	Id_Node* node = id_node_find(global_context.id_table, id);
	if (node == NULL) {
		fprintf(stderr, "ERRO: Variavel `%s` nao foi declarada\n", id);
		global_context.errors++;
	}
	else {
		node->used = true;
        codeGenerator(operation, data_location());
	}
}

void check_unused_variables() {
    if (!global_context.id_table) return;
    Id_Node *node = global_context.id_table;
    while (node) {
        if (!node->used) {
            fprintf(stderr, "WARNING: Variável `%s` não foi utilizada\n", node->id);
            global_context.warnings += 1;
        }
        node = node->nxt;
    }
}


struct labels {
    int label_goto;
    int label_jmp_false;
};
struct labels * labelSpaceAllocation() {
    struct labels *lbl = malloc(sizeof(struct labels));
    if (!lbl) {
        fprintf(stderr, "Erro: falha na alocação de memória para labels\n");
        exit(EXIT_FAILURE);
    }
    lbl->label_goto = -1;
    lbl->label_jmp_false = -1;
    return lbl;
}

%}

%union semrec{
    int ival;
    char *sval;
    struct labels *lval; // For backpatching
}


/* Declaração dos tokens retornados pelo scanner */
%start program
/* Declaração dos tokens e suas associações de tipo */
%token <sval> IDENTIFIER
%token <ival> NUMBER
%token <lval> IF GOTO

/* Declaração dos tipos de dados para as não-terminais */
%type <ival> exp
%type <sval> command
%type <lval> command_sequence
%token LET IN END INTEGER SKIP READ WRITE THEN ELSE FI WHILE DO
%token EQ LT GT ADD SUB MUL DIV EXP ASSIGN
%left ADD SUB
%left MUL DIV
%right EXP
%nonassoc EQ LT GT
%left '(' ')'

%%

/* Regras da gramática */

program:
    LET declarations IN {codeGenerator(OP_DATA, data_location()-1);} command_sequence END {
        codeGenerator(OP_HALT, 0); 
        YYACCEPT;
    }
;

declarations: /* empty */
    | INTEGER id_seq IDENTIFIER '.'                                            { add_new_indentifier($3);}
;

id_seq:
    | id_seq  IDENTIFIER   ','                                      { add_new_indentifier($2); }
;

command_sequence:                    
    command ';'                     { $$ = $1; }
    | command_sequence command ';'   { $$ = $2; }
;

command: SKIP
    | IDENTIFIER ASSIGN exp ';' { printf("ASSIGN %s t%d\n", $1, $3); check_identifier_context(OP_STORE,$1);}
    | IF exp {$1 = (struct labels *) labelSpaceAllocation(); $1->label_jmp_false = reserve_loc();} 
      THEN command_sequence {$1->label_goto = reserve_loc();} ELSE {}
      FI ';' { printf("IF t%d ... THEN ... FI\n", $2); back_patch($1->label_goto, OP_GOTO, 0);}
    | WHILE exp DO command_sequence END ';' { printf("WHILE t%d ... DO ...\n", $2); }
    | READ IDENTIFIER ';' { printf("READ %s\n", $2); check_identifier_context(OP_READ_INT, $2);}
    | WRITE exp { printf("WRITE t%d\n", $2); codeGenerator(OP_WRITE_INT, 0);}
    ;

exp:
      NUMBER { $$ = temp_count++; printf("t%d = %d\n", $$, $1);  codeGenerator(OP_LD_INT, $1); }
    | IDENTIFIER { 
        $$ = temp_count++; 
        printf("t%d = %s\n", $$, $1); 
        if (id_node_find(global_context.id_table, $1) == NULL) {
            fprintf(stderr, "ERRO: Variável `%s` não foi declarada corretamente\n", $1);
            exit(EXIT_FAILURE);
        }
        check_identifier_context(OP_LD_VAR, $1);
    }
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
	check_unused_variables();
    
    printf("Código gerado até agora (code_offset = %d):\n", code_offset);
    if (global_context.errors  == 0){
        print_code();
        fetch_execute_cycle();
    }
    if (file != NULL) {
        fclose(file);
    }
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