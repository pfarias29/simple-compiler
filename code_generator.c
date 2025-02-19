#include <stdlib.h>
#include "code_generator.h"
#include "math.h"
#include <string.h>
#include "int_code_generator.h"
#include <stdbool.h>

int offsetTM = 0;
int gp = 5;                        // Global Pointer
int mp = 6;                        // Memory Pointer
int acc1 = 1;
int pcr = 7;
struct instructionTM TMcode[999];
int symbolOffset = 0;
char symbol[50];

struct argument firstArgument;
struct argument secondArgument;

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
   int reg; 
   int temp = 0;
   int jump_back_to = 0;
   symbolOffset = 0;

   do { 
      inst_struct = code[pc];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               filePrintROCode(file,"HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               if (code[pc].arg1 <= 2) {
                  filePrintROCode(file,"IN", code[pc].arg1 + 2, 0, 0);
               } else {
                  filePrintROCode(file,"IN", acc, 0, 0);
                  filePrintRMCode(file,"ST", acc, acc+code[pc].arg1, gp);
               }
               break;
            case OP_WRITE_INT : 
               if (code[pc].arg1 <= 2) {
                  filePrintROCode(file,"OUT", code[pc-1].arg1 + 2, 0, 0);
               } else {
                  filePrintROCode(file,"OUT", acc, 0, 0);
               }
               break;
            case OP_ADD :
               if (code[pc+1].arg1 <= 2) {
                  reg = code[pc+1].arg1 + 2;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"ADD", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"ADD", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"ADD", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"ADD", reg, acc1, acc);
               }
               break;
            case OP_SUB : 
               if (code[pc+1].arg1 <= 2) {
                  reg = code[pc+1].arg1 + 2;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"SUB", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"SUB", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"SUB", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"SUB", reg, acc1, acc);
               }
               break;
            case OP_MUL: 
               if (code[pc+1].arg1 <= 2) {
                  reg = code[pc+1].arg1 + 2;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"MUL", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"MUL", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"MUL", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"MUL", reg, acc1, acc);
               }
               break;
            case OP_DIV : 
               if (code[pc+1].arg1 <= 2) {
                  reg = code[pc+1].arg1 + 2;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"DIV", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"DIV", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"DIV", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"DIV", reg, acc1, acc);
               }
               break;
            case OP_EXP : // Não vou implementar esse!!!
                break;
            case OP_STORE : 
               if (code[pc].arg1 <= 2) {
                  reg = code[pc].arg1 + 2;
               } else {
                  filePrintRMCode(file,"ST", acc, acc+code[pc].arg1, gp);
               }
               break;
            case OP_JMP_FALSE : 
               //filePrintRMCode(file,"JEQ", acc, code_offset-pc-2, pcr);
               break;
            case OP_GOTO : // Uncoditional Jump
               filePrintRMCode(file,"LDA", pcr, 0, pcr);
               break;
            case OP_DATA : // Não foi implementado
               filePrintRMCode(file,"LD", mp, 0, 0);
               filePrintRMCode(file,"ST", 0, 0, 0); 
               break;
            case OP_LD_INT : 
               filePrintRMCode(file,"LDC", acc, inst_struct.arg1, 0);
               if (inst_struct.partOfOperation == 1) {
                  filePrintRMCode(file,"ST", acc, 0, mp);
               }
               if (inst_struct.partOfOperation == 2) {
                  filePrintRMCode(file,"LD", acc1, 0, mp);
               }
               break;
            case OP_LD_VAR : 
               filePrintRMCode(file,"LD", acc, acc+inst_struct.arg1, gp);
               if (inst_struct.partOfOperation == 1) {
                  if (code[pc].arg1 <= 2) {
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = code[pc].arg1 + 2;
                  } else {
                     firstArgument.isReg = false;
                     filePrintRMCode(file,"ST", acc, 0, mp);
                  }
               }
               if (inst_struct.partOfOperation == 2) {
                  if (code[pc].arg1 <= 2) {
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = code[pc].arg1 + 2;
                  } else {
                     secondArgument.isReg = false;
                     filePrintRMCode(file,"LD", acc1, 0, mp);
                  }
               } 
               break;
            case OP_LT : // Não foi implementado
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"SUB", acc, acc1, acc);
               }
               filePrintRMCode(file,"JGE", acc, code_offset-pc-5, pcr);
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"SUB", acc, acc1, acc);
               }
               filePrintRMCode(file,"JNE", acc, code_offset-pc-5, pcr);
               break; 
            case OP_GT : // Não foi implementado
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  filePrintROCode(file,"SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  filePrintROCode(file,"SUB", acc, acc1, acc);
               }
               filePrintRMCode(file,"JLE", acc, code_offset-pc-5, pcr);
               break; 
            default : 
               break;
      }
      pc++;
   } while (inst_struct.opcode != OP_HALT);
   fclose(file);
   int i = 0 ;
   while (i < offsetTM) {
      printf("%3d:  %5s  %d,%d,%d \n", i, TMcode[i].opcode, TMcode[i].targetRegister, TMcode[i].fstRegister, TMcode[i].sndRegister);
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
   do { 
      inst_struct = code[pc];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               termPrintROCode("HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               termPrintROCode("IN", acc, 0, 0);
               termPrintRMCode("ST", acc, acc+code[pc].arg1, gp);
               break;
            case OP_WRITE_INT : 
               termPrintROCode("OUT", acc, 0, 0);
               break;
            case OP_ADD : 
               termPrintROCode("ADD", acc, acc1, acc);
               break;
            case OP_SUB : 
               termPrintROCode("SUB", acc, acc1, acc);
               break;
            case OP_MUL: 
               termPrintROCode("MUL", acc, acc1, acc);
               break;
            case OP_DIV : 
               termPrintROCode("DIV", acc, acc1, acc);
               break;
            case OP_EXP : // Não vou implementar esse!!!
                break;
            case OP_STORE : 
               termPrintRMCode("ST", acc, acc+code[pc].arg1, gp);
               break;
            case OP_JMP_FALSE : 
               //termPrintRMCode("JEQ", acc, code_offset-pc-2, pcr);
               break;
            case OP_GOTO : // Uncoditional Jump
               termPrintRMCode("LDA", pcr, 0, pcr);
               break;
            case OP_DATA :
               termPrintRMCode("LD", mp, 0, 0);
               termPrintRMCode("ST", 0, 0, 0);   
               break;
            case OP_LD_INT : 
               termPrintRMCode("LDC", acc, inst_struct.arg1, 0);
               if (inst_struct.partOfOperation == 1) {
                  termPrintRMCode("ST", acc, 0, mp);
               }
               if (inst_struct.partOfOperation == 2) {
                  termPrintRMCode("LD", acc1, 0, mp);
               } 
               break;
            case OP_LD_VAR : 
               termPrintRMCode("LD", acc, acc+inst_struct.arg1, gp);
               if (inst_struct.partOfOperation == 1) {
                  termPrintRMCode("ST", acc, 0, mp);
               }
               if (inst_struct.partOfOperation == 2) {
                  termPrintRMCode("LD", acc1, 0, mp);
               } 
               break;
            case OP_LT : 
               jump_back_to = pc;
               termPrintROCode("SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               termPrintRMCode("JGE", acc, code_offset-pc-5, pcr);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               termPrintROCode("SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               termPrintRMCode("JNE", acc, code_offset-pc-5, pcr);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               break; 
            case OP_GT : 
               jump_back_to = pc;
               termPrintROCode("SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               termPrintRMCode("JLE", acc, code_offset-pc-5, pcr);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               break; 
            default : 
               break;
      }
      pc++;
   } while (inst_struct.opcode != OP_HALT);
}