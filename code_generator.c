#include <stdlib.h>
#include "code_generator.h"
#include "math.h"


/* OPERATIONS: External Representations */
// Intermediate Code
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int","lt", "eq", "gt", "add", "sub", "mul", "div", "exp" };
struct instruction code[999];
struct instruction TMcode[999];
int stack[999];

int pc = 0;                      // Program Counter
int pcTM = 0;
struct instruction inst_struct;  // Instruction Struct
int acc = 0;                      // Accumulator
int accTM = 0;
int acc1TM = 0;
int top = 0;                     // Last
int topTM = 0;

// Stack Machine by Aaby
void fetch_execute_cycle() { 
   do {
      inst_struct = code[pc++];
      switch (inst_struct.opcode) {
         case OP_HALT: 
            printf( "halt\n" ); break;
         case OP_READ_INT: 
            printf( "Input: " );
            scanf( "%d", &stack[acc + inst_struct.arg1] ); break;
         case OP_WRITE_INT : 
            printf( "Output: %d\n", stack[top--] ); break;
         case OP_STORE : 
            stack[inst_struct.arg1] = stack[top--]; break;
         case OP_JMP_FALSE : 
            if ( stack[top--] == 0 ){
               pc = inst_struct.arg1;
            }
            break;
         case OP_GOTO : 
            pc = inst_struct.arg1; break;
         case OP_DATA : 
            top = top + inst_struct.arg1; break;
         case OP_LD_INT : 
            stack[++top] = inst_struct.arg1; break;
         case OP_LD_VAR : 
            stack[++top] = stack[acc + inst_struct.arg1]; break;
         case OP_LT : 
            if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
            else { stack[--top] = 0;}
            break; 
         case OP_EQ : 
            if ( stack[top-1] == stack[top] ) { stack[--top] = 1; }
            else { stack[--top] = 0;}
            break; 
         case OP_GT : 
            if ( stack[top-1] > stack[top] ) { stack[--top] = 1; }
            else { stack[--top] = 0;}
            break; 
         case OP_ADD : 
            stack[top-1] = stack[top-1] + stack[top];
            top--;
            break;
         case OP_SUB : 
            stack[top-1] = stack[top-1] - stack[top];
            top--;
            break;
         case OP_MUL: 
            stack[top-1] = stack[top-1] * stack[top];
            top--;
            break;
         case OP_DIV : 
            stack[top-1] = stack[top-1] / stack[top];
            top--;
            break;
         case OP_EXP : 
            stack[top-1] = pow(stack[top-1], stack[top]);
            top--;
            break;
         default : 
            break;
      }
   } while (inst_struct.opcode != OP_HALT);
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

void codeGenerator(enum code_ops operation, int arg){
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

// Impressão do código intermediário 
// N: Opcode argumento
void print_code() {
   int i = 0;
   printf("CÓDIGO INTERMEDIÁRIO\n");
   while (i < code_offset) {
      printf("%d: %-10s%4d\n", i , op_name[code[i].opcode], code[i].arg1 );
      i++;
   }
}
