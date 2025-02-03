/* DECLARATIONS */ 
/* OPERATIONS: Internal Representation */
#include <stdio.h>

enum code_ops { HALT, STORE, JMP_FALSE, GOTO, DATA, LD_INT, LD_VAR, READ_INT, WRITE_INT, LT, 
                EQ, GT, ADD, SUB, MULT, DIV, PWR };
//OPERATIONS: External Representation
char *op_name[] = {"halt", "store", "jmp_false", "goto", "data", "ld_int", "ld_var", "in_int", "out_int", "lt", 
                    "eq", "gt", "add", "sub", "mult", "div", "pwr" };
struct instruction
{
    enum code_ops op;
    int arg;
};

/* CODE Array */
struct instruction code[999];

/* RUN-TIME Stack */
int stack[999];

/* Registers */
int pc = 0;             // Program Counter (endereço da próxima instrução a ser executada)
struct instruction ir;  // Instruction Register (guarda a instrução atual) 
int ar = 0;             // Activation Record (usado para variáveis)
int top = 0;            // Índice do topo da pilha
char ch;                // Não sei o que é?

/* Fetch Execute Cycle */
void fetch_execute_cycle()
{ do { 
    printf( "PC = %3d IR.arg = %8d AR = %3d Top = %3d,%8d\n", pc, ir.arg, ar, top, stack[top]);
    /* Fetch */
    ir = code[pc++];
    /* Execute */
    switch (ir.op) {
        case HALT :         printf( "halt\n" ); break;
        case READ_INT :     printf( "Input: " );
                            scanf( "%ld", &stack[ar+ir.arg] ); break;
        case WRITE_INT :    printf( "Output: %d\n", stack[top--] ); break;
        case STORE :        stack[ir.arg] = stack[top--]; break;
        case JMP_FALSE :    if ( stack[top--] == 0 )
                                pc = ir.arg;
                                break;
        case GOTO :         pc = ir.arg; break;
        case DATA :         top = top + ir.arg; break;
        case LD_INT :       stack[++top] = ir.arg; break;
        case LD_VAR :       stack[++top] = stack[ar+ir.arg]; break;
        case LT :           if ( stack[top-1] < stack[top] )
                                stack[--top] = 1;
                            else stack[--top] = 0;
                                break;
        case EQ :           if ( stack[top-1] == stack[top] )
                                stack[--top] = 1;
                            else stack[--top] = 0;
                                break;
        case GT :           if ( stack[top-1] > stack[top] )
                                stack[--top] = 1;
                            else stack[--top] = 0;
                                break;
        case ADD :          stack[top-1] = stack[top-1] + stack[top];
                            top--;
                            break;
        case SUB :          stack[top-1] = stack[top-1] - stack[top];
                            top--;
                            break;
        case MULT :         stack[top-1] = stack[top-1] * stack[top];
                            top--;
                            break;
        case DIV :          stack[top-1] = stack[top-1] / stack[top];
                            top--;
                            break;
        case PWR :          stack[top-1] = stack[top-1] * stack[top];
                            top--;
                            break;
        default :           printf( "%sInternal Error: Memory Dump\n" );
                            break;
        }
    }
    while (ir.op != HALT);
}