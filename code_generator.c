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

void generateCode(char* tmfile) { 
    FILE * file;
    file = fopen(tmfile,"w");
    pc = 0;                      // Program Counter
    acc = 0;                      // Accumulator
    acc1 = 1;
    int jump_back_to = 0;
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
      switch (inst_struct.opcode) {
            case OP_HALT: 
                strcpy(TMcode[offsetTM].opcode, "HALT");
                TMcode[offsetTM].targetRegister = 0;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_READ_INT: 
               stack[acc + inst_struct.arg1] = 0;
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
                TMcode[offsetTM].targetRegister = acc;
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
               int pctemp = pc + 1;
               strcpy(TMcode[offsetTM].opcode, "JEQ");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 2;
               TMcode[offsetTM].sndRegister = pctemp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = acc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pctemp;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = pctemp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = acc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "JEQ");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 2;
               TMcode[offsetTM].sndRegister = pctemp;
               fprintf(file, "%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               if ( stack[top--] == 0 ){
                  pc = inst_struct.arg1;
               } else {
                  pc = pctemp - 1;
               }
               break;
            case OP_GOTO : // Uncoditional Jump
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pc;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = pc;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_DATA : // Não foi implementado
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               stack[++top] = inst_struct.arg1;
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
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
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_LD_VAR : 
               stack[++top] = stack[acc + inst_struct.arg1]; 
               strcpy(TMcode[offsetTM].opcode, "LD");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = acc;
               TMcode[offsetTM].sndRegister = mp;
               fprintf(file,"%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_LT : // Não foi implementado
               jump_back_to = pc;
               if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               strcpy(TMcode[offsetTM].opcode, "SUB");
               TMcode[offsetTM].fstRegister = acc1;
               TMcode[offsetTM].sndRegister = acc;
               TMcode[offsetTM].targetRegister = acc;
               fprintf(file,"%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
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
 }

 void generateCodeTerminal() { 
    pc = 0;                      // Program Counter
    acc = 0;                      // Accumulator
    acc1 = 1;
    int jump_back_to = 0;
    strcpy(TMcode[offsetTM].opcode, "LD");
    TMcode[offsetTM].targetRegister = mp;
    TMcode[offsetTM].fstRegister = 0;
    TMcode[offsetTM].sndRegister = 0;
    printf("%3d:  %5s  %d,%d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
    strcpy(TMcode[offsetTM].opcode, "ST");
    TMcode[offsetTM].targetRegister = 0;
    TMcode[offsetTM].fstRegister = 0;
    TMcode[offsetTM].sndRegister = 0;
    printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
    do { 
      inst_struct = code[pc++];
      switch (inst_struct.opcode) {
            case OP_HALT: 
                strcpy(TMcode[offsetTM].opcode, "HALT");
                TMcode[offsetTM].targetRegister = 0;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_READ_INT: 
               stack[acc + inst_struct.arg1] = 0;
               strcpy(TMcode[offsetTM].opcode, "IN");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = 0;
               printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "ST");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = acc;
               TMcode[offsetTM].sndRegister = gp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_WRITE_INT : 
                strcpy(TMcode[offsetTM].opcode, "OUT");
                TMcode[offsetTM].targetRegister = acc;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_ADD : 
                strcpy(TMcode[offsetTM].opcode, "ADD");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] + stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_SUB : 
                strcpy(TMcode[offsetTM].opcode, "SUB");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] - stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_MUL: 
                strcpy(TMcode[offsetTM].opcode, "MUL");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] * stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_DIV : 
                strcpy(TMcode[offsetTM].opcode, "DIV");
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                stack[top-1] = stack[top-1] / stack[top];
                TMcode[offsetTM].targetRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
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
                printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_JMP_FALSE : 
               int pctemp = pc + 1;
               strcpy(TMcode[offsetTM].opcode, "JEQ");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 2;
               TMcode[offsetTM].sndRegister = pctemp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = acc;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pctemp;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = pctemp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 1;
               TMcode[offsetTM].sndRegister = acc;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "JEQ");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = 2;
               TMcode[offsetTM].sndRegister = pc;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               if ( stack[top--] == 0 ){
                  pc = inst_struct.arg1;
               } else {
                  pc = pctemp - 1;
               }
               break;
            case OP_GOTO : // Uncoditional Jump
               strcpy(TMcode[offsetTM].opcode, "LDA");
               TMcode[offsetTM].targetRegister = pc;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = pc;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_DATA :
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               stack[++top] = inst_struct.arg1;
               strcpy(TMcode[offsetTM].opcode, "LDC");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = stack[++top];
               TMcode[offsetTM].sndRegister = 0;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "ST");
               TMcode[offsetTM].targetRegister = 0;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = mp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               strcpy(TMcode[offsetTM].opcode, "LD");
               TMcode[offsetTM].targetRegister = 0;
               TMcode[offsetTM].fstRegister = 0;
               TMcode[offsetTM].sndRegister = gp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_LD_VAR : 
               stack[++top] = stack[acc + inst_struct.arg1]; 
               strcpy(TMcode[offsetTM].opcode, "LD");
               TMcode[offsetTM].targetRegister = acc;
               TMcode[offsetTM].fstRegister = acc;
               TMcode[offsetTM].sndRegister = mp;
               printf("%3d:  %5s  %d,%7d(%d) \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
               break;
            case OP_LT : 
               jump_back_to = pc;
               if ( stack[top-1] < stack[top] ) { stack[--top] = 1; }
               else { stack[--top] = 0;}
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               strcpy(TMcode[offsetTM].opcode, "SUB");
               TMcode[offsetTM].fstRegister = acc1;
               TMcode[offsetTM].sndRegister = acc;
               TMcode[offsetTM].targetRegister = acc;
               printf("%3d:  %5s  %d,%d,%d \n",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
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