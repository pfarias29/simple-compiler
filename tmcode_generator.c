#include <stdlib.h>
#include <math.h>

char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int","lt", "eq", "gt", "add", "sub", "mul", "div", "exp" };
//struct instruction inst_struct;  // Instruction Struct

int offsetTM = 0;
int pc = 0;                      // Program Counter
int acc = 0;                      // Accumulator
int gp = 5;                        // Global Pointer
int mp = 6;                        // Memory Pointer

struct instructionTM {
    char opcode[10];
    int targetRegister;
    int fstRegister;        // Offset or 1st Source Register
    int sndRegister;        // 2nd Source Register
};
struct instructionTM TMcode[999];

void fetch_execute_cycleTM(int stack[999], struct instruction code) { 
    do {
       inst_struct = code[pc++];
       switch (inst_struct.opcode) {
            case OP_HALT: 
                printf( "halt\n" );
                strcpy(TMcode[offsetTM].opcode, "HALT");
                TMcode[offsetTM].targetRegister = 0;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d,%d ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_READ_INT: 
                printf( "Input: " );
                scanf( "%d", &stack[acc + inst_struct.arg1] );
                strcpy(TMcode[offsetTM].opcode, "IN");
                TMcode[offsetTM].targetRegister = acc;
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d,%d ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                strcpy(TMcode[offsetTM].opcode, "ST");
                TMcode[offsetTM].targetRegister = acc;
                TMcode[offsetTM].fstRegister = inst_struct.arg1;
                TMcode[offsetTM].sndRegister = gp;
                printf("%3d:  %5s  %d,%d,%d ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
            case OP_WRITE_INT : 
                printf( "Output: %d\n", stack[top--] );
                strcpy(TMcode[offsetTM].opcode, "OUT");
                TMcode[offsetTM].targetRegister = stack[top--];
                TMcode[offsetTM].fstRegister = 0;
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d,%d ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
            case OP_ADD : 
                strcpy(TMcode[offsetTM].opcode, "ADD");
                TMcode[offsetTM].targetRegister = stack[top-1];
                stack[top-1] = stack[top-1] + stack[top];
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d(%d) ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_SUB : 
                strcpy(TMcode[offsetTM].opcode, "SUB");
                TMcode[offsetTM].targetRegister = stack[top-1];
                stack[top-1] = stack[top-1] - stack[top];
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d(%d) ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_MUL: 
                strcpy(TMcode[offsetTM].opcode, "MUL");
                TMcode[offsetTM].targetRegister = stack[top-1];
                stack[top-1] = stack[top-1] * stack[top];
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d(%d) ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_DIV : 
                strcpy(TMcode[offsetTM].opcode, "DIV");
                TMcode[offsetTM].targetRegister = stack[top-1];
                stack[top-1] = stack[top-1] / stack[top];
                TMcode[offsetTM].fstRegister = stack[top];
                TMcode[offsetTM].sndRegister = stack[top-1];
                printf("%3d:  %5s  %d,%d(%d) ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                top--;
                break;
            case OP_EXP : 
                stack[top-1] = pow(stack[top-1], stack[top]);
                top--;
                break;
            case OP_STORE : 
                stack[inst_struct.arg1] = stack[top--]; 
                strcpy(TMcode[offsetTM].opcode, "ST");
                TMcode[offsetTM].targetRegister = inst_struct.arg1;
                TMcode[offsetTM].fstRegister = stack[top--];
                TMcode[offsetTM].sndRegister = 0;
                printf("%3d:  %5s  %d,%d(%d) ",offsetTM++,TMcode[offsetTM].opcode, TMcode[offsetTM].targetRegister, TMcode[offsetTM].fstRegister, TMcode[offsetTM].sndRegister);
                break;
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
             stack[++top] = stack[acc + inst_struct.arg1]; break;
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
          default : 
             break;
       }
    } while (inst_struct.opcode != OP_HALT);
 }