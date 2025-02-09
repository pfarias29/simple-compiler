#include <stdlib.h>
#include "code_generator.h"
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
            stack[++top] = stack[acc + inst_struct.arg1]; 
            break;
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

int offsetTM = 0;
int gp = 5;                        // Global Pointer
int mp = 6;                        // Memory Pointer

struct instructionTM {
    char opcode[10];
    int targetRegister;
    int fstRegister;        // Offset or 1st Source Register
    int sndRegister;        // 2nd Source Register
};
struct instructionTM TMcode[999];

void fetch_execute_cycleTM(char* tmfile) { 
   FILE * file;
   file = fopen(tmfile,"w");
   pc = 0;                      // Program Counter
   acc = 0;                      // Accumulator
   strcpy(TMcode[offsetTM].opcode, "LD");
    TMcode[offsetTM].targetRegister = mp;
    TMcode[offsetTM].fstRegister = 0;
    TMcode[offsetTM].sndRegister = 0;
    fprintf(file,"%3d:  %5s  %d,%d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
    strcpy(TMcode[offsetTM].opcode, "ST");
    TMcode[offsetTM].targetRegister = 0;
    TMcode[offsetTM].fstRegister = 0;
    TMcode[offsetTM].sndRegister = 0;
    fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
   do {
      inst_struct = code[pc++];
      printf("%d\n", inst_struct.opcode);
      switch (inst_struct.opcode) {
            case OP_HALT: 
                strcpy(TMcode[offsetTM].opcode, "HALT");
                TMcode[offsetTM].targetRegister = 0;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_READ_INT: 
                scanf( "%d", &stack[acc + inst_struct.arg1] );
                strcpy(TMcode[offsetTM].opcode, "IN");
                TMcode[offsetTM].targetRegister = acc;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                strcpy(TMcode[offsetTM].opcode, "ST");
                TMcode[offsetTM].targetRegister = acc;
                TMcode[offsetTM].fstRegister = acc;
                TMcode[offsetTM].sndRegister = gp;
                fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_WRITE_INT : 
                strcpy(TMcode[offsetTM].opcode, "OUT");
                TMcode[offsetTM].targetRegister = stack[top--];
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_ADD : 
                strcpy(TMcode[offsetTM].opcode, "ADD");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] + stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_SUB : 
                strcpy(TMcode[offsetTM].opcode, "SUB");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] - stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_MUL: 
                strcpy(TMcode[offsetTM].opcode, "MUL");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] * stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_DIV : 
                strcpy(TMcode[offsetTM].opcode, "DIV");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] / stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_EXP : // Não vou implementar esse!!!
                stack[top-1] = pow(stack[top-1], stack[top]);
                top--;
                break;
            case OP_STORE : 
                stack[inst_struct.arg1] = stack[top--]; 
                strcpy(TMcode[offsetTM].opcode, "ST");
                TMcode[offsetTM].targetRegister = inst_struct.arg1;
                TMcode[offsetTM].fstRegister = stack[top--];
                TMcode[offsetTM].sndRegister = 0;
                fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_JMP_FALSE : 
               if ( stack[top--] == 0 ){
                  pc = inst_struct.arg1;
                  }
               break;
            case OP_GOTO : // Uncoditional Jump
               pc = inst_struct.arg1; 
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pc;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = pc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_DATA :
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               stack[++top] = inst_struct.arg1;
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = inst_struct.arg1;
               TMcode[offsetTM].fstRegister = stack[++top];
               TMcode[offsetTM].sndRegister = 0;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "ST");
               TMcode[offsetTM].targetRegister = 0;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = mp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LD");
               TMcode[offsetTM].targetRegister = 0;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = gp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM]);
               break;
            case OP_LD_VAR : 
               stack[++top] = stack[acc + inst_struct.arg1]; 
               strcpy(TMcode[offsetTM].opcode, "LD");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = stack[acc + inst_struct.arg1];
               TMcode[offsetTM].sndRegister = gp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_LT : 
               if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            case OP_EQ : 
               int temp1 = stack[top-1];
               int temp2 = stack[top];
               int temp3 = top;
               strcpy(TMcode[offsetTM].opcode, "SUB");
               TMcode[offsetTM].fstRegister = temp1;
               TMcode[offsetTM].sndRegister = temp2;
               temp1 = temp1 - temp2;
               TMcode[offsetTM].targetRegister = temp1;
               fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "JEQ");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 2;
               TMcode[offsetTM].sndRegister = pc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               if ( stack[top-1] == stack[top] ) { 
                  stack[--top] = 1; 
               }
               else { stack[--top] = 0;}
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = stack[--top];
               TMcode[offsetTM].sndRegister = acc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pc;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = pc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = stack[--top];
               TMcode[offsetTM].sndRegister = acc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break; 
            case OP_GT : 
               if ( stack[top-1] > stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            default : 
               break;
      }
   } while (inst_struct.opcode != OP_HALT);
   fclose(file);
 }