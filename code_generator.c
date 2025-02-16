#include <stdlib.h>
#include "code_generator.h"
#include "math.h"
#include <string.h>
#include "int_code_generator.h"

int offsetTM = 0;
int gp = 5;                        // Global Pointer = tamanhodaMemoria
int mp = 6;                        // Memory Pointer = fp
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
               filePrintROCode(file,"IN", acc, 0, 0);          //  REGISTRADOR A SER SALVO,IGNORED,IGNORED 
               filePrintRMCode(file,"ST", acc, 0, gp);       // STORE GUARDANDO NA POS ERRADA      //ST VALOR A SER SALVO REG, POSIÇÃO, OFFSET
               break;
            case OP_WRITE_INT : 
               filePrintROCode(file,"OUT", acc1, 0, 0);         //OUT REGISTRADOR,IGNORED,IGNORED
               break;
            case OP_ADD : 
               filePrintRMCode(file, "LD", acc1, 0, mp);       //LD VALOR SALVO REG, POSICAO DE MEM, OFFSET
               filePrintROCode(file,"ADD",  acc, acc1, acc);   //ADD REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               break;
            case OP_SUB : 
               filePrintRMCode(file, "LD", acc1, 0, mp);       //LD VALOR SALVO REG, POSICAO DE MEM, OFFSET
               filePrintROCode(file,"SUB",  acc, acc1, acc);   //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               top--;
               break;
            case OP_MUL: 
               filePrintRMCode(file, "LD", acc1, 0, mp);       //LD VALOR SALVO REG, POSICAO DE MEM, OFFSET
               filePrintROCode(file,"MUL",  acc, acc1, acc);   //MUL REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               break;
            case OP_DIV : 
               filePrintRMCode(file, "LD", acc1, 0, mp);       //LD VALOR SALVO REG, POSICAO DE MEM, OFFSET
               filePrintROCode(file,"DIV",  acc, acc1, acc);   //DIV REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               break;
            case OP_EXP : // Não vou implementar esse!!!
                break;
            case OP_STORE : 
               filePrintRMCode(file,"ST", acc, 0, gp);         //ST VALOR A SER SALVO REG, REGISTRADOR, OFFSET
               break;
            case OP_JMP_FALSE : // OPCODE r,d(s)
               int pctemp = pc + 1;
               int skipAddress = 0;
               while (inst_struct.opcode != OP_GOTO){
                  inst_struct = code[pctemp++];
                  switch (inst_struct.opcode)
                  {
                  case OP_READ_INT:
                     skipAddress = skipAddress + 2;
                     break;
                  case OP_WRITE_INT:
                     skipAddress++;
                     break;
                  case OP_ADD:
                     skipAddress = skipAddress + 2;
                     break;   
                  case OP_SUB:
                     skipAddress = skipAddress + 2;
                     break;
                  case OP_MUL:
                     skipAddress = skipAddress + 2;
                     break;
                  case OP_DIV:
                     skipAddress = skipAddress + 2;
                     break;
                  case OP_EXP:
                     /* code */
                     break;
                  case OP_STORE:
                     skipAddress++;
                     break;
                  case OP_JMP_FALSE:
                     skipAddress++;
                     break;
                  case OP_DATA:
                     /* code */
                     break;
                  case OP_LD_INT:
                     skipAddress++;
                     break;
                  case OP_LD_VAR:
                     skipAddress++;
                     break;
                  case OP_LT:
                     skipAddress = skipAddress + 5;
                     break;
                  case OP_EQ:
                     skipAddress = skipAddress + 5;
                     break;
                  case OP_GT:
                     skipAddress = skipAddress + 5;
                     break;
                  
                  default:
                     break;
                  }
               }
               
               
               filePrintRMCode(file,"JEQ", acc, skipAddress, 7);  //mudar pc   //JEQ REGISTRADOR, ?, LABEL           
               jump_back_to = pctemp;
               break;
            case OP_GOTO : // Uncoditional Jump
               filePrintRMCode(file,"LDA", jump_back_to, 0, jump_back_to);       //LDA REGISTRADOR, VALOR, ?
               break;
            case OP_DATA : // Não foi implementado
               top = top + inst_struct.arg1; break;
            case OP_LD_INT : 
               filePrintRMCode(file,"LDC", acc, inst_struct.arg1, 0);   //LDC REGISTRADOR, VALOR DA CONSTANTE, OFFSET  
               //filePrintRMCode(file, "ST", acc, acc, mp);               //ST VALOR A SER SALVO REG, POSIÇÃO, OFFSET
               break;
            case OP_LD_VAR : 
               filePrintRMCode(file,"LD", acc1, 0, gp);         //LD VALOR SALVO REG, POSICAO DE MEM, OFFSET
               break;
            case OP_LT : // Não foi implementado
               jump_back_to = pc;
               filePrintROCode(file,"SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               filePrintRMCode(file,"JLT", acc, 2, 7);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               filePrintRMCode(file,"LDC", acc, 0, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED  
               filePrintRMCode(file,"LDA", 7, 1, 7);       //LDA JUMP INCONDICONAL +1                         CARREGA D + REG[S] EM REG[R]
               filePrintRMCode(file,"LDC", acc, 1, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED       //LDC SERVE PARA O ELSE
               
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               filePrintROCode(file,"SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               filePrintRMCode(file,"JEQ", acc, 2, 7);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               filePrintRMCode(file,"LDC", acc, 0, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED  
               filePrintRMCode(file,"LDA", 7, 1, 7);       //LDA JUMP INCONDICONAL +1                         CARREGA D + REG[S] EM REG[R]
               filePrintRMCode(file,"LDC", acc, 1, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED       //LDC SERVE PARA O ELSE
               break; 
            case OP_GT : // Não foi implementado
               jump_back_to = pc;
               filePrintROCode(file,"SUB", acc, acc1, acc);       //SUB REGISTRADOR A SER SALVO, REGISTRADOR 1, REGISTRADOR 2
               filePrintRMCode(file,"JGT", acc, 2, 7);     //JEQ REGISTRADOR == 0, 2 -> tamanho do pulo, ENDEREÇO DE PC
               filePrintRMCode(file,"LDC", acc, 0, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED  
               filePrintRMCode(file,"LDA", 7, 1, 7);       //LDA JUMP INCONDICONAL +1                         CARREGA D + REG[S] EM REG[R]
               filePrintRMCode(file,"LDC", acc, 1, acc);   //LDC ACC, VALOR DA CONSTANTE,IGNORED       //LDC SERVE PARA O ELSE
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
   do { 
      inst_struct = code[pc++];
      switch (inst_struct.opcode) {
            case OP_HALT: 
               termPrintROCode("HALT", 0, 0, 0);
               break;
            case OP_READ_INT: 
               termPrintROCode("IN", acc, 0, 0);
               termPrintRMCode("ST", acc, acc, gp);
               break;
            case OP_WRITE_INT : 
               termPrintROCode("OUT", acc, 0, 0);
               break;
            case OP_ADD : 
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("ADD", acc, acc1, acc);
               break;
            case OP_SUB : 
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("SUB", acc, acc1, acc);
               break;
            case OP_MUL: 
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("MUL", acc, acc1, acc);
               break;
            case OP_DIV : 
               termPrintRMCode("LD", acc1, 0, mp);
               termPrintROCode("DIV", acc, acc1, acc);
               break;
            case OP_EXP : // Não vou implementar esse!!!
                break;
            case OP_STORE : 
               termPrintRMCode("ST", acc, 0, gp);
               break;
            case OP_JMP_FALSE : 
               int pctemp = pc + 1;
               termPrintRMCode("JEQ", acc, 2, pctemp);
               termPrintRMCode("LDC", acc, 0, acc);
               termPrintRMCode("LDA", pctemp, 1, pctemp);
               termPrintRMCode("LDC", acc, 1, acc);
               termPrintRMCode("JEQ", acc, 2, pctemp);
               jump_back_to = inst_struct.arg1;
               break;
            case OP_GOTO : // Uncoditional Jump
               //termPrintRMCode("LDA", jump_back_to, 0, jump_back_to);
               break;
            case OP_DATA :
               termPrintRMCode("LD", mp, 0, 0);
               termPrintRMCode("ST", 0, 0, 0);   
               break;
            case OP_LD_INT : 
               termPrintRMCode("LDC", acc, inst_struct.arg1, 0);
               termPrintRMCode("ST", acc, acc, mp);
               break;
            case OP_LD_VAR : 
               termPrintRMCode("LD", acc, acc, gp);
               termPrintRMCode("ST", acc, acc, mp);
               break;
            case OP_LT : 
               jump_back_to = pc;
               break; 
            case OP_EQ : 
               jump_back_to = pc;
               termPrintRMCode("LD", acc1, acc, mp);
               termPrintRMCode("SUB", acc, acc1, acc);
               break; 
            case OP_GT : 
               jump_back_to = pc;
               break; 
            default : 
               break;
      }
   } while (inst_struct.opcode != OP_HALT);
 }