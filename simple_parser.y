%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "stack_machine.h"
#include "code_generator.h"
#include "symbol_table.h"
#include "simple_parser.tab.h"

extern FILE *yyin;
extern int yylex(void);
extern int yylineno; // Linha atual do analisador léxico
extern char* yytext; // Texto atual do analisador léxico
void yyerror(const char *s);
int errors; /* Error Count */
struct lbs /* Labels for data, if and while */
{
	int for_goto;
	int for_jmp_false;
};

struct lbs * newlblrec() /* Allocate space for the labels */
{
	return (struct lbs *) malloc(sizeof(struct lbs));
}


install ( char *sym_name )
{
	symrec *s;
	s = getsym (sym_name);
	if (s == 0)
		s = putsym (sym_name);
	else { 
		errors++;
		printf( "%s is already defined\n", sym_name );
	}
}

context_check( enum code_ops operation, char *sym_name )
{ 
	symrec *identifier;
	identifier = getsym( sym_name );
	if ( identifier == 0 )
	{ 
		errors++;
		printf( "%s", sym_name );
		printf( "%s\n", " is an undeclared identifier" );
	}
	else gen_code( operation, identifier->offset );
}

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

void print_context(Context *ctx) {
    printf("Contexto Global:\n");
    printf("Erros: %d\n", ctx->errors);
    printf("Warnings: %d\n", ctx->warnings);
    printf("Tabela de Identificadores:\n");

    Id_Node *node = ctx->id_table;
    while (node != NULL) {
        printf("ID: %s | Usado: %s\n", node->id, node->used ? "Sim" : "Não");
        node = node->nxt;
    }
}

%}

%union semrec // Valores semanticos
{
	int val;
	char *id;
	struct lbs *lbls; 	/* For backpatching */
}

/* Declaração dos tokens retornados pelo scanner */
%start program
%token <id>  IDENTIFIER 
%token <val> NUMBER
%token <lbls> IF WHILE /* For backpatching labels */
%token LET IN END INTEGER SKIP READ WRITE THEN ELSE FI DO
%token ASSIGN
%left ADD SUB
%left MUL DIV
%right EXP
%nonassoc EQ LT GT
%left '(' ')'

%%

/* Regras da gramática */

program:
    LET 
		declarations IN { gen_code ( DATA, sym_table->offset);}
		command_sequence 
		END                        { gen_code(HALT, 0); YYACCEPT;}
;

declarations: /* empty */
    | INTEGER id_seq IDENTIFIER '.'{ add_new_indentifier($3); }
;

id_seq:
    IDENTIFIER { add_new_indentifier($1); }
    | id_seq ',' IDENTIFIER { add_new_indentifier($3); }
;

command_sequence:
    command                      
    | command_sequence command                                      {}
;

command:
    SKIP ';'                                                        {}
    | IDENTIFIER ASSIGN exp ';'                                     { check_identifier($1); }
    | IF exp 	{ 
					$1 = (struct lbs *) newlblrec();
					$1->for_jmp_false = reserve_loc();
				}
		THEN command_sequence { $1->for_goto = reserve_loc(); }
		ELSE command_sequence { back_patch( $1->for_jmp_false, JMP_FALSE,gen_label() );}
		FI ';'     { back_patch( $1->for_goto, GOTO, gen_label() );}
    | WHILE { 	$1 = (struct lbs *) newlblrec();
				$1->for_goto = gen_label(); }
				exp { $1->for_jmp_false = reserve_loc(); }
				DO command_sequence END { gen_code( GOTO, $1->for_goto );
									back_patch( $1->for_jmp_false, JMP_FALSE, gen_label() ); 
			}
    | READ IDENTIFIER ';'                                           { check_identifier($2); }
    | WRITE exp ';'                                                 { gen_code(WRITE, 0); }
;

exp:
    NUMBER 	{ gen_code( LD_INT, $1 ); }
    | IDENTIFIER                                                    { check_identifier($1); }
    | exp '<' exp { gen_code( LT, 0 ); }
	| exp '=' exp { gen_code( EQ, 0 ); }
	| exp '>' exp { gen_code( GT, 0 ); }
	| exp '+' exp { gen_code( ADD, 0 ); }
	| exp '-' exp { gen_code( SUB, 0 ); }
	| exp '*' exp { gen_code( MULT, 0 ); }
	| exp '/' exp { gen_code( DIV, 0 ); }
	| exp '^' exp { gen_code( PWR, 0 ); }
	| '(' exp ')'
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

	if (global_context.warnings > 0) {
		fprintf(stdout, "Compilacao terminada com %d warnings\n", global_context.warnings);
	}

	if (global_context.errors > 0) {
		fprintf(stdout, "Compilacao terminada com %d erros\n", global_context.errors);
		return 1;
	}
	// Imprimir tabela de identificadores antes de encerrar
	print_context(&global_context);

	fprintf(stdout, "Compilacao terminada com sucesso\n");

    return 0;
}
