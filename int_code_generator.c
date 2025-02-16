#include <stdlib.h>
#include "int_code_generator.h"
#include "math.h"
#include <string.h>


/* OPERATIONS: External Representations */
// Intermediate Code
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int", "lt", "eq", "gt", "add", "sub", "mul", "div", "exp" };
struct instruction code[999];
int stack[999];

int pc = 0;                      // Program Counter
struct instruction inst_struct;  // Instruction Struct
struct symbolTable st[10];
int acc = 0;                      // Accumulator
int top = 0;                     // Last
int variables_count = -2;
int symbolCounter = 0;

// Impressão do código intermediário 
// N: Opcode argumento
void print_intermediate_code() {
   int i = 0;
   printf("CÓDIGO INTERMEDIÁRIO\n");
   while (i < code_offset) {
      printf("%d: %-10s%4d\n", i , op_name[code[i].opcode], code[i].arg1);
      i++;
   }
}

int data_offset = 0; /* Initial offset */
int data_location(char symbol[50]) {
   int i = 0;
   while (i <= symbolCounter){
      // printf("st[i].id = %d st[i].variable %s\n", st[i].id, st[i].variable);
      if (strcmp(st[i].variable, symbol) == 0){
         data_offset = st[i].id;
         break;
      }
      if (i == symbolCounter){
         st[i].id = symbolCounter++;
         data_offset = st[i].id;
         strcpy(st[i].variable, symbol);
         break;
      }
      i++;
   }
   return data_offset++;
}
int code_offset = 0; /* Initial offset */
int gen_label() {
   return code_offset;
}
int reserve_loc() {
   return code_offset++;
}

void intermediateCodeGenerator(enum code_ops operation, int arg, char variable[50]){
// void intermediateCodeGenerator(enum code_ops operation, int arg){
   if (code_offset >= 999) {
      fprintf(stderr, "Erro: Code array out of bounds\n");
      exit(1);
   }
   code[code_offset].opcode = operation;
   code[code_offset].arg1 = arg;
   // printf("%d %d\n", code[code_offset].opcode, code[code_offset].arg1);
   if (operation == 5){    //LD_INT
      code[code_offset].varOrConstOrOp = 1;  
      strcpy(code[code_offset].variable, "");
      code[code_offset].partOfOperation = 0;
   } else if (operation == 6 || operation == 7){      //LD_VAR  e IN_INT
      code[code_offset].varOrConstOrOp = 0;
      strcpy(code[code_offset].variable, variable);
      code[code_offset].partOfOperation = 0;
   } else {
      if (operation > 8 && operation < 17){
         code[code_offset].partOfOperation = 1;
         code[code_offset-2].partOfOperation = 1;
         code[code_offset-1].partOfOperation = 2;
      }
      code[code_offset].varOrConstOrOp = 2;
   }
   code_offset++;
}

void back_patch( int addr, enum code_ops operation, int arg ) {
   code[addr].opcode = operation;
   code[addr].arg1 = arg;
}