#include <stdlib.h>
#include "int_code_generator.h"
#include "math.h"
#include <string.h>


/* OPERATIONS: External Representations */
// Intermediate Code
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int","lt", "eq", "gt", "add", "sub", "mul", "div", "exp" };
struct instruction code[999];
int stack[999];

int pc = 0;                      // Program Counter
struct instruction inst_struct;  // Instruction Struct
int acc = 0;                      // Accumulator
int top = 0;                     // Last

// Impressão do código intermediário 
// N: Opcode argumento
void print_intermediate_code() {
   int i = 0;
   printf("CÓDIGO INTERMEDIÁRIO\n");
   while (i < code_offset) {
      printf("%d: %-10s%4d\n", i , op_name[code[i].opcode], code[i].arg1 );
      i++;
   }
}

int data_offset = 0; /* Initial offset */
int data_location() {
   return data_offset++;
}
int code_offset = 0; /* Initial offset */
int gen_label() {
   return code_offset;
}
int reserve_loc() {
   return code_offset++;
}

void intermediateCodeGenerator(enum code_ops operation, int arg){
   if (code_offset >= 999) {
      fprintf(stderr, "Erro: Code array out of bounds\n");
      exit(1);
   }
   code[code_offset].opcode = operation;
   code[code_offset++].arg1 = arg;
}

void back_patch( int addr, enum code_ops operation, int arg ) {
   code[addr].opcode = operation;
   code[addr].arg1 = arg;
}