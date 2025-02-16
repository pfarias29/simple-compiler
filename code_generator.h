#define MAX_INSTRUCTIONS 999
#include <stdio.h>

extern int offsetTM;
extern int gp;
extern int mp;
extern int acc1;
// extern int symbolCounter;

struct instructionTM {
    char opcode[10];
    int targetRegister;
    int fstRegister;
    int sndRegister;
};

// struct symbolTable{
//     int id;
//     char variable[50];
// };

extern struct instructionTM TMcode[MAX_INSTRUCTIONS];

void filePrintRMCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void filePrintROCode(FILE *file, char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void generateCode(char* tmfile);
void termPrintRMCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void termPrintROCode(char opcode[10], int targetRegister, int fstRegister, int sndRegister);
void generateCodeTerminal();