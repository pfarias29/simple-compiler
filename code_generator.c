#include <stdlib.h>
#include "code_generator.h"
#include "math.h"
#include <string.h>
#include "int_code_generator.h"

int offsetTM = 0;
int gp = 5;                        // Global Pointer
int mp = 6;                        // Memory Pointer
int acc1 = 1;
struct instructionTM TMcode[999];

// Printa Instruções RM no arquivo .tm
void filePrintRMCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister) {
   strcpy(TMcode[offsetTM].opcode, opcode);
   TMcode[offsetTM].targetRegister = targetRegister;
   TMcode[offsetTM].fstRegister = fstRegister;
   TMcode[offsetTM].sndRegister = sndRegister;
   fprintf(file, "%3d:  %5s  %d,%7d(%d) \n", offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
}

// Printa Instruções RO no arquivo .tm
void filePrintROCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister) {
   strcpy(TMcode[offsetTM].opcode, opcode);
   TMcode[offsetTM].targetRegister = targetRegister;
   TMcode[offsetTM].fstRegister = fstRegister;
   TMcode[offsetTM].sndRegister = sndRegister;
   fprintf(file, "%3d:  %5s  %d,%d,%d \n", offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
}

// Função que gera código em linguagem do TMVS. Essa é a partir de um arquivo
void generateCode(char* tmfile) { 
   FILE * file;
   file = fopen(tmfile,"w");
   pc = 0;                      // Program Counter
   acc = 0;                      // Accumulator 1
   acc1 = 1;                     // Accumulator 2
   int temp = 0;
   int jump_back_to = 0;
   filePrintRMCode(file,"LD", mp, 0, 0);
   filePrintRMCode(file,"ST", 0, 0, 0);
   do { 
      inst_struct = code[pc++];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               filePrintROCode(file,"HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               stack[acc + inst_struct.arg1] = 0;
               filePrintROCode(file,"IN", acc, 0, 0);
               filePrintRMCode(file,"ST", acc, acc, gp);
               break;
            case OP_WRITE_INT : 
               filePrintROCode(file,"OUT", acc, 0, 0);
               break;
            case OP_ADD : 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] + stack[top];
               filePrintRMCode(file, "LD", acc1, 0, mp);
               filePrintROCode(file,"ADD",  acc, acc1, acc);
               break;
            case OP_SUB : 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] - stack[top];
               filePrintRMCode(file, "LD", acc1, 0, mp);
               filePrintROCode(file,"SUB",  acc, acc1, acc);
               top--;
               break;
            case OP_MUL: 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] * stack[top];
               filePrintRMCode(file, "LD", acc1, 0, mp);
               filePrintROCode(file,"MUL",  acc, acc1, acc);
               top--;
               break;
            case OP_DIV : 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] / stack[top];
               filePrintRMCode(file, "LD", acc1, 0, mp);
               filePrintROCode(file,"DIV",  acc, acc1, acc);
               top--;
               break;
            case OP_EXP : // Não vou implementar esse!!!
                stack[top-1] = pow(stack[top-1], stack[top]);
                top--;
                break;
            case OP_STORE : 
               stack[inst_struct.arg1] = stack[top--]; 
               filePrintRMCode(file,"ST", acc, 0, gp);
               break;
            case OP_JMP_FALSE : 
               int pctemp = pc + 1;
               filePrintRMCode(file,"JEQ", acc, 2, pctemp);
               filePrintRMCode(file,"LDC", acc, 0, acc);
               filePrintRMCode(file,"LDA", pctemp, 1, pctemp);
               filePrintRMCode(file,"LDC", acc, 1, acc);
               filePrintRMCode(file,"JEQ", acc, 2, pctemp);
               if ( stack[top--] == 0 ){
                  jump_back_to = pctemp;
               }
               break;
            case OP_GOTO : // Uncoditional Jump
               filePrintRMCode(file,"LDA", jump_back_to, 0, jump_back_to);
               break;
            case OP_DATA : // Não foi implementado
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               stack[++top] = inst_struct.arg1;
               filePrintRMCode(file,"LDC", acc, inst_struct.arg1, 0);
               filePrintRMCode(file, "ST", acc, acc, mp);
               break;
            case OP_LD_VAR : 
               stack[++top] = stack[acc + inst_struct.arg1]; 
               filePrintRMCode(file,"LD", acc, acc, gp);
               break;
            case OP_LT : // Não foi implementado
               jump_back_to = pc;
               if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               filePrintRMCode(file, "LD", acc1, acc, mp);
               filePrintROCode(file,"SUB", acc, acc1, acc);
               if ( stack[top-1] == stack[top] ) { 
                  stack[--top] = 1; 
               }
               else { stack[--top] = 0;}
               break; 
            case OP_GT : // Não foi implementado
               jump_back_to = pc;
               if ( stack[top-1] > stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            default : 
               break;
      }
   } while (inst_struct.opcode != OP_HALT);
   fclose(file);
   int i = 0 ;
   while (i < offsetTM){
      printf("%3d:  %5s  %d,%d,%d \n",i,TMcode[i].opcode, TMcode[i].targetRegister, TMcode[i].fstRegister, TMcode[i].sndRegister);
      i++;
   }
 }

//  Printa Instruções RM
void termPrintRMCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister) {
   strcpy(TMcode[offsetTM].opcode, opcode);
   TMcode[offsetTM].targetRegister = targetRegister;
   TMcode[offsetTM].fstRegister = fstRegister;
   TMcode[offsetTM].sndRegister = sndRegister;
   printf("%3d:  %5s  %d,%7d(%d) \n", offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
}

// Printa Instruções RO
void termPrintROCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister) {
   strcpy(TMcode[offsetTM].opcode, opcode);
   TMcode[offsetTM].targetRegister = targetRegister;
   TMcode[offsetTM].fstRegister = fstRegister;
   TMcode[offsetTM].sndRegister = sndRegister;
   printf("%3d:  %5s  %d,%d,%d \n", offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
}

// Função que gera código em linguagem do TMVS. Essa é a partir do input de terminal
void generateCodeTerminal() { 
   pc = 0;                      // Program Counter
   acc = 0;                      // Accumulator 1
   acc1 = 1;                     // Accumulator 2
   int jump_back_to = 0;
   int temp = 0;
   termPrintRMCode("LD", mp, 0, 0);
   termPrintRMCode("ST", 0, 0, 0);
   do { 
      inst_struct = code[pc++];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               termPrintROCode("HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               stack[acc + inst_struct.arg1] = 0;
               termPrintROCode("IN", acc, 0, 0);
               termPrintRMCode("ST", acc, acc, gp);
               break;
            case OP_WRITE_INT : 
               termPrintROCode("OUT", acc, 0, 0);
               break;
            case OP_ADD : 
               stack[top-1] = stack[top-1] + stack[top];
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("ADD", acc, acc1, acc);
               top--;
               break;
            case OP_SUB : 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] - stack[top];
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("SUB", acc, acc1, acc);
               top--;
               break;
            case OP_MUL: 
               int temp = stack[top-1];
               stack[top-1] = stack[top-1] * stack[top];
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("MUL", acc, acc1, acc);
               top--;
               break;
            case OP_DIV : 
               temp = stack[top-1];
               stack[top-1] = stack[top-1] / stack[top];
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("DIV", acc, acc1, acc);
               top--;
               break;
            case OP_EXP : // Não vou implementar esse!!!
                stack[top-1] = pow(stack[top-1], stack[top]);
                top--;
                break;
            case OP_STORE : 
               stack[inst_struct.arg1] = stack[top--]; 
               termPrintRMCode("ST", acc, 0, gp);
               break;
            case OP_JMP_FALSE : 
               int pctemp = pc + 1;
               termPrintRMCode("JEQ", acc, 2, pctemp);
               termPrintRMCode("LDC", acc, 0, acc);
               termPrintRMCode("LDA", pctemp, 1, pctemp);
               termPrintRMCode("LDC", acc, 1, acc);
               termPrintRMCode("JEQ", acc, 2, pctemp);
               if ( stack[top--] == 0 ){
                  jump_back_to = inst_struct.arg1;
               }
               break;
            case OP_GOTO : // Uncoditional Jump
               termPrintRMCode("LDA", jump_back_to, 0, jump_back_to);
               break;
            case OP_DATA :
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               stack[++top] = inst_struct.arg1;
               termPrintRMCode("LDC", acc, inst_struct.arg1, 0);
               termPrintRMCode("ST", acc, acc, mp);
               break;
            case OP_LD_VAR : 
               stack[++top] = stack[acc + inst_struct.arg1]; 
               termPrintRMCode("LD", acc, acc, gp);
               termPrintRMCode("ST", acc, acc, mp);
               break;
            case OP_LT : 
               jump_back_to = pc;
               if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               termPrintRMCode("LD", acc1, acc, mp);
               termPrintRMCode("SUB", acc, acc1, acc);
               if ( stack[top-1] == stack[top] ) { 
                  stack[--top] = 1; 
               }
               else { stack[--top] = 0;}
               break; 
            case OP_GT : 
               jump_back_to = pc;
               if ( stack[top-1] > stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            default : 
               break;
      }
   } while (inst_struct.opcode != OP_HALT);
 }