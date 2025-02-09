#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_INSTRUCTIONS 999

// Definição dos códigos de operação
enum code_ops { OP_HALT, OP_STORE, OP_JMP_FALSE, OP_GOTO, OP_DATA, OP_LD_INT, OP_LD_VAR, OP_READ_INT, OP_WRITE_INT, OP_LT, OP_EQ, OP_GT, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_EXP };

// Estrutura de uma instrução
struct instruction {
    enum code_ops opcode;
    int arg1;
};

// Variáveis globais
extern struct instruction code[MAX_INSTRUCTIONS];
extern int stack[MAX_INSTRUCTIONS];
extern int pc;
extern struct instruction inst_struct;
extern int acc;
extern int top;
extern int data_offset;
extern int code_offset;

// Protótipos das funções
void intermediateCodeGenerator();
int data_location();
int gen_label();
int reserve_loc();
void intermediateCodeGenerator(enum code_ops operation, int arg);
void back_patch(int addr, enum code_ops operation, int arg);
void print_code();