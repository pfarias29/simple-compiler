#define MAX_INSTRUCTIONS 999

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

void generateCode(char* tmfile);