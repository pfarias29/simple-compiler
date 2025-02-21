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

   // Inicializa o array de contagem de frequência
   int freq[100] = {0}; // Supondo que os valores de arg1 variem de 0 a 99

// Função de comparação para qsort
int compare(const void *a, const void *b) {
    int freqA = ((int*)a)[1];
    int freqB = ((int*)b)[1];
    return freqB - freqA; // Ordem decrescente
}

// Conta a frequência de cada arg1
for (int i = 0; i < code_offset; i++) {
    if (code[i].opcode == OP_LD_VAR || code[i].opcode == OP_STORE || code[i].opcode == OP_READ_INT) {
        freq[code[i].arg1]++;
    }
}

// Cria um array de pares (arg1, frequência)
int freqPairs[100][2];
for (int i = 0; i < 100; i++) {
    freqPairs[i][0] = i; // arg1
    freqPairs[i][1] = freq[i]; // frequência
}

// Ordena os pares pela frequência
qsort(freqPairs, 100, sizeof(freqPairs[0]), compare);

// Lista os valores de arg1 que mais aparecem
printf("Frequência de arg1 (ordenada):\n");
for (int i = 0; i < 100; i++) {
    if (freqPairs[i][1] > 0) {
        printf("arg1: %d, frequência: %d\n", freqPairs[i][0], freqPairs[i][1]);
    }
}

int jumpTo(int pc) {
   int pctemp = pc;
   int skipAddress = 0;
      while (inst_struct.opcode != OP_GOTO){
         inst_struct = code[pctemp++];
         switch (inst_struct.opcode)
         {
         case OP_READ_INT:
            if (code[pc].arg1 == freqPairs[0][0] || code[pc].arg1 == freqPairs[1][0] || code[pc].arg1 == freqPairs[2][0]){
               skipAddress++;
            } else {
               skipAddress = skipAddress + 2;
            }
            break;
         case OP_WRITE_INT:
            skipAddress++;
            break;
         case OP_ADD:
            skipAddress++;
            break;   
         case OP_SUB:
            skipAddress++;
            break;
         case OP_MUL:
            skipAddress++;
            break;
         case OP_DIV:
            skipAddress++;
            break;
         case OP_EXP:
            /* code */
            break;
         case OP_STORE:
            if (code[pc].arg1 == freqPairs[0][0] || code[pc].arg1 == freqPairs[1][0] || code[pc].arg1 == freqPairs[2][0]){
            break;
            } else {
               skipAddress++;
            }
            break;
         case OP_JMP_FALSE:
            break;
         case OP_DATA:
            skipAddress = skipAddress + 2;
            break;
         case OP_LD_INT:
            skipAddress = skipAddress + 5;
            break;
         case OP_LD_VAR:
            skipAddress++;
               if (inst_struct.partOfOperation == 1) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 4;
                  } else {
                     firstArgument.isReg = false;
                     skipAddress++;
                  }
               }
               if (inst_struct.partOfOperation == 2) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 4;
                  } else {
                     secondArgument.isReg = false;
                     skipAddress++;
                  }
               } 
            break;
         case OP_LT:
            skipAddress = skipAddress + 2;
            break;
         case OP_EQ:
            skipAddress = skipAddress + 2;
            break;
         case OP_GT:
            skipAddress = skipAddress + 2;
            break;
         
         default:
            break;
         }
      }
   return skipAddress;
}


   do { 
      inst_struct = code[pc];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               filePrintROCode(file,"HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               if (code[pc].arg1 == freqPairs[0][0]){
                  filePrintROCode(file,"IN", 2, 0, 0);
               } else if (code[pc].arg1 == freqPairs[1][0]){
                  filePrintROCode(file,"IN", 3, 0, 0);
               } else if (code[pc].arg1 == freqPairs[2][0]){
                  filePrintROCode(file,"IN", 4, 0, 0);
               } else {
                  filePrintROCode(file,"IN", acc, 0, 0);
                  filePrintRMCode(file,"ST", acc, acc+code[pc].arg1, gp);
               }
               break;

            case OP_WRITE_INT : 
               if (code[pc-1].arg1 == freqPairs[0][0]){
                  filePrintROCode(file,"OUT", 2, 0, 0);
               } else if (code[pc-1].arg1 == freqPairs[1][0]){
                  filePrintROCode(file,"OUT", 3, 0, 0);
               } else if (code[pc-1].arg1 == freqPairs[2][0]){
                  filePrintROCode(file,"OUT", 4, 0, 0);
               } else {
                  filePrintROCode(file,"OUT", acc, 0, 0);
               }
               break;

            case OP_ADD :
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
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
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
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
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
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
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
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
               if (code[pc].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc].arg1 == freqPairs[2][0]){
                  reg = 4;
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
                  if (code[pc].arg1 == freqPairs[0][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 4;
                  } else {
                     firstArgument.isReg = false;
                     filePrintRMCode(file,"ST", acc, 0, mp);
                  }
               }
               if (inst_struct.partOfOperation == 2) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 4;
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
               filePrintRMCode(file,"JGE", acc, jumpTo(pc), pcr);
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
               filePrintRMCode(file,"JNE", acc, jumpTo(pc), pcr);
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
               filePrintRMCode(file,"JLE", acc, jumpTo(pc), pcr);
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
   int reg;

   // Inicializa o array de contagem de frequência
   int freq[100] = {0}; // Supondo que os valores de arg1 variem de 0 a 99

// Função de comparação para qsort
int compare(const void *a, const void *b) {
    int freqA = ((int*)a)[1];
    int freqB = ((int*)b)[1];
    return freqB - freqA; // Ordem decrescente
}

// Conta a frequência de cada arg1
for (int i = 0; i < code_offset; i++) {
    if (code[i].opcode == OP_LD_VAR || code[i].opcode == OP_STORE || code[i].opcode == OP_READ_INT) {
        freq[code[i].arg1]++;
    }
}

// Cria um array de pares (arg1, frequência)
int freqPairs[100][2];
for (int i = 0; i < 100; i++) {
    freqPairs[i][0] = i; // arg1
    freqPairs[i][1] = freq[i]; // frequência
}

// Ordena os pares pela frequência
qsort(freqPairs, 100, sizeof(freqPairs[0]), compare);

// Lista os valores de arg1 que mais aparecem
printf("Frequência de arg1 (ordenada):\n");
for (int i = 0; i < 100; i++) {
    if (freqPairs[i][1] > 0) {
        printf("arg1: %d, frequência: %d\n", freqPairs[i][0], freqPairs[i][1]);
    }
}

int jumpTo(int pc) {
   int pctemp = pc;
   int skipAddress = 0;
      while (inst_struct.opcode != OP_GOTO){
         inst_struct = code[pctemp++];
         switch (inst_struct.opcode)
         {
         case OP_READ_INT:
            if (code[pc].arg1 == freqPairs[0][0] || code[pc].arg1 == freqPairs[1][0] || code[pc].arg1 == freqPairs[2][0]){
               skipAddress++;
            } else {
               skipAddress = skipAddress + 2;
            }
            break;
         case OP_WRITE_INT:
            skipAddress++;
            break;
         case OP_ADD:
            skipAddress++;
            break;   
         case OP_SUB:
            skipAddress++;
            break;
         case OP_MUL:
            skipAddress++;
            break;
         case OP_DIV:
            skipAddress++;
            break;
         case OP_EXP:
            /* code */
            break;
         case OP_STORE:
            if (code[pc].arg1 == freqPairs[0][0] || code[pc].arg1 == freqPairs[1][0] || code[pc].arg1 == freqPairs[2][0]){
            break;
            } else {
               skipAddress++;
            }
            break;
         case OP_JMP_FALSE:
            break;
         case OP_DATA:
            skipAddress = skipAddress + 2;
            break;
         case OP_LD_INT:
            skipAddress = skipAddress + 5;
            break;
         case OP_LD_VAR:
            skipAddress++;
               if (inst_struct.partOfOperation == 1) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 4;
                  } else {
                     firstArgument.isReg = false;
                     skipAddress++;
                  }
               }
               if (inst_struct.partOfOperation == 2) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 4;
                  } else {
                     secondArgument.isReg = false;
                     skipAddress++;
                  }
               } 
            break;
         case OP_LT:
            skipAddress = skipAddress + 2;
            break;
         case OP_EQ:
            skipAddress = skipAddress + 2;
            break;
         case OP_GT:
            skipAddress = skipAddress + 2;
            break;
         
         default:
            break;
         }
      }
   return skipAddress;
}

   do { 
      inst_struct = code[pc];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               termPrintROCode("HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               if (code[pc].arg1 == freqPairs[0][0]){
                  termPrintROCode("IN", 2, 0, 0);
               } else if (code[pc].arg1 == freqPairs[1][0]){
                  termPrintROCode("IN", 3, 0, 0);
               } else if (code[pc].arg1 == freqPairs[2][0]){
                  termPrintROCode("IN", 4, 0, 0);
               } else {
                  termPrintROCode("IN", acc, 0, 0);
                  termPrintRMCode("ST", acc, acc+code[pc].arg1, gp);
               }
               break;

            case OP_WRITE_INT : 
               if (code[pc-1].arg1 == freqPairs[0][0]){
                  termPrintROCode("OUT", 2, 0, 0);
               } else if (code[pc-1].arg1 == freqPairs[1][0]){
                  termPrintROCode("OUT", 3, 0, 0);
               } else if (code[pc-1].arg1 == freqPairs[2][0]){
                  termPrintROCode("OUT", 4, 0, 0);
               } else {
                  termPrintROCode("OUT", acc, 0, 0);
               }
               break;

            case OP_ADD :
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
               } else {
                  reg = acc;
               }
               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("ADD", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("ADD", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("ADD", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("ADD", reg, acc1, acc);
               }
               break;
            case OP_SUB : 
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("SUB", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("SUB", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("SUB", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("SUB", reg, acc1, acc);
               }
               break;
            case OP_MUL: 
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("MUL", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("MUL", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("MUL", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("MUL", reg, acc1, acc);
               }
               break;
            case OP_DIV : 
               if (code[pc+1].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc+1].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc+1].arg1 == freqPairs[2][0]){
                  reg = 4;
               } else {
                  reg = acc;
               }

               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("DIV", reg, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("DIV", reg, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("DIV", reg, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("DIV", reg, acc1, acc);
               }
               break;
            case OP_EXP : // Não vou implementar esse!!!
                break;
            case OP_STORE : 
               if (code[pc].arg1 == freqPairs[0][0]){
                  reg = 2;
               } else if (code[pc].arg1 == freqPairs[1][0]){
                  reg = 3;
               } else if (code[pc].arg1 == freqPairs[2][0]){
                  reg = 4;
               } else {
                  termPrintRMCode("ST", acc, acc+code[pc].arg1, gp);
               }
               break;
            case OP_JMP_FALSE : 
               //termPrintRMCode("JEQ", acc, code_offset-pc-2, pcr);
               break;
            case OP_GOTO : // Uncoditional Jump
               termPrintRMCode("LDA", pcr, 0, pcr);
               break;
            case OP_DATA : // Não foi implementado
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
                  if (code[pc].arg1 == freqPairs[0][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     firstArgument.isReg = true;
                     firstArgument.regPartOfOperation = 4;
                  } else {
                     firstArgument.isReg = false;
                     termPrintRMCode("ST", acc, 0, mp);
                  }
               }
               if (inst_struct.partOfOperation == 2) {
                  if (code[pc].arg1 == freqPairs[0][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 2;
                  } else if (code[pc].arg1 == freqPairs[1][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 3;
                  } else if (code[pc].arg1 == freqPairs[2][0]){
                     secondArgument.isReg = true;
                     secondArgument.regPartOfOperation = 4;
                  } else {
                     secondArgument.isReg = false;
                     termPrintRMCode("LD", acc1, 0, mp);
                  }
               } 
               break;
            case OP_LT : // Não foi implementado
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("SUB", acc, acc1, acc);
               }
               termPrintRMCode("JGE", acc, jumpTo(pc), pcr);
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("SUB", acc, acc1, acc);
               }
               termPrintRMCode("JNE", acc, jumpTo(pc), pcr);
               break; 
            case OP_GT : // Não foi implementado
               jump_back_to = pc;
               if (firstArgument.isReg && secondArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, secondArgument.regPartOfOperation);
               } else if (firstArgument.isReg) {
                  termPrintROCode("SUB", acc, firstArgument.regPartOfOperation, acc);
               } else if (secondArgument.isReg) {
                  termPrintROCode("SUB", acc, acc1, secondArgument.regPartOfOperation);
               } else {
                  termPrintROCode("SUB", acc, acc1, acc);
               }
               termPrintRMCode("JLE", acc, jumpTo(pc), pcr);
               break; 
            default : 
               break;
      }
      pc++;
   } while (inst_struct.opcode != OP_HALT);
}