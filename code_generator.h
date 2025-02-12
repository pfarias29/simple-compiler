#define MAX_INSTRUCTIONS 999
#include <stdio.h>

extern int offsetTM;
extern int gp;
extern int mp;
extern int acc1;

struct instructionTM {
    char opcode[10];
    int targetRegister;
    int fstRegister;
    int sndRegister;
};

extern struct instructionTM TMcode[MAX_INSTRUCTIONS];

void filePrintRMCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void filePrintROCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void generateCode(char* tmfile);
void termPrintRMCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void termPrintROCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void generateCodeTerminal();