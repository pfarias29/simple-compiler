#include <stdlib.h>
#include "code_generator.h"
#include "math.h"


/* OPERATIONS: External Representation */
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int","lt", "eq", "gt", "add", "sub", "mul", "div", "exp" };
struct instruction code[999];
int stack[999];

int pc = 0;             // Program Counter
struct instruction ir;  // Instruction Struct
int ar = 0;             // Accumulator
int top = 0;            // Last

void fetch_execute_cycle() { 
   do {
      ir = code[pc++];
      switch (ir.opcode) {
         case OP_HALT: 
            printf( "halt\n" ); break;
         case OP_READ_INT: 
            printf( "Input: " );
            scanf( "%d", &stack[ar+ir.arg] ); break;
         case OP_WRITE_INT : 
            printf( "Output: %d\n", stack[top--] ); break;
         case OP_STORE : 
            stack[ir.arg] = stack[top--]; break;
         case OP_JMP_FALSE : 
            if ( stack[top--] == 0 ){
               pc = ir.arg;
            }
            break;
         case OP_GOTO : 
            pc = ir.arg; break;
         case OP_DATA : 
            top = top + ir.arg; break;
         case OP_LD_INT : 
            stack[++top] = ir.arg; break;
         case OP_LD_VAR : 
            stack[++top] = stack[ar+ir.arg]; break;
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
            printf( "%sInternal Error: Memory Dump\n" );
            break;
      }
   } while (ir.opcode != OP_HALT);
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
      fprintf(stderr, "Error: Code array out of bounds\n");
      exit(1);
   }
   code[code_offset].opcode = operation;
   code[code_offset++].arg = arg;
}

void back_patch( int addr, enum code_ops operation, int arg ) {
   code[addr].opcode = operation;
   code[addr].arg = arg;
}

void print_code() {
   int i = 0;
   printf("CÓDIGO OBJETO\n");
   while (i < code_offset) {
      printf("%3d: %-10s%4d\n", i , op_name[code[i].opcode], code[i].arg );
      i++;
   }
}
