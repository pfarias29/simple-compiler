#include <stdio.h>

enum code_ops { OP_HALT, OP_STORE, OP_JMP_FALSE, OP_GOTO, OP_DATA, OP_LD_INT, OP_LD_VAR, OP_READ_INT, OP_WRITE_INT, OP_LT, OP_EQ, OP_GT, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_EXP };

extern char *op_name[];

struct instruction {
    enum code_ops opcode;
    int arg;
};

extern struct instruction code[999];
extern int stack[999];
extern int pc;
extern struct instruction ir;
extern int ar;
extern int top;

void fetch_execute_cycle();

extern int data_offset;
int data_location();

extern int code_offset;
int gen_label();
int reserve_loc();

void codeGenerator(enum code_ops opcode, int *arg);
void back_patch(int addr, enum code_ops operation, int arg);
void print_code();