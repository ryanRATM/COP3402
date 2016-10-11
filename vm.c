#include <stdio.h>
#include <stdlib.h>

# define MAX_STACK_HEIGHT 2000
# define MAX_CODE_LENGTH 500
# define MAX_LEXI_LEVELS 3

typedef struct d{
    int op;   // opcode
    int  l;   // L
    int  m;   // M
} instruction;

// stack to hold memory
int* stack;
// hold all lines of code in program need to run
instruction* instructions;

// instructions that can call
const char* INSTRUCTION_SET [10] = {"", "LIT", "OPR", "LOD", "STO",
                                    "CAL", "INC", "JMP", "JPC", "SIO"};
// operations ALU can do
const char* ALU_OPERATIONS [14] = {"RET", "NEG", "ADD", "SUB", "MUL",
                                    "DIV", "ODD", "MOD", "EQL", "NEQ",
                                    "LSS", "LEQ", "GTR", "GEQ"};

// all 4 registers in PL/0 machine
int sp;
int bp;
int pc;
instruction ir;



// will do the work each operation does, pass the operation
// to do, and what going to work on
void operation();

// does mathematical operations
void ALUoperations(int operation);

// pass the stack and base pointer, will be a recursive function
void printStack(int bp, int sp);

// print operation
void printOperation();
// go L levels down to the pointer and go to the bp
int base(int level, int bp);


int main(int argc, char** argv) {

    // allocate space
    stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
    instructions = calloc(MAX_CODE_LENGTH + 1, sizeof(instruction));

    // file read in
    FILE* file = fopen(argv[1], "r");
    int i;
    for(i = 0; !feof(file); i++) {
        fscanf(file, "%d %d %d", &instructions[i].op, &instructions[i].l, &instructions[i].m);
    }
    fclose(file);

    // first part of assignment, print out code
    for(pc = 0; pc < i - 1; pc++) {
        ir = instructions[pc];
        printOperation();
        printf("\n");
    }
    pc = 0;
    bp = 1;
    sp = 0;

    printf("\n%s\n\n", "Execution:");


    printf("                  %4s %4s %4s   stack\n", "pc", "bp", "sp");
    printf("                  %4d %4d %4d\n", pc, bp, sp);
    while(pc != (i - 1)) {

        // fetch instruction
        ir = instructions[pc];
        // print operation
        printOperation();

        // increment pc
        pc++;

        operation();

        // print letter, bp, sp, ir, ccc
        printf("%4d %4d %4d   ", pc, bp, sp);

        // call printStack(bp, sp);
        printStack(bp, sp);
        printf("\n");



    }

    return 0;
}


void printOperation() {
    // need if statements for SIO and OPR operations
    if(ir.op == 9 && ir.m == 2) {
        printf("%3d %s           ", pc, "HLT");
    } else if(ir.op == 2) {
        printf("%3d %s           ", pc, ALU_OPERATIONS[ir.m]);
    } else if(ir.op == 4 || ir.op == 5) {
        printf("%3d %s %4d  %4d", pc, INSTRUCTION_SET[ir.op], ir.l, ir.m);
    } else {
        printf("%3d %s       %4d", pc, INSTRUCTION_SET[ir.op], ir.m);
    }
}

// recursive function, call all the way down
void printStack(int bp, int sp) {
    // base case, if very first AR on stack
    if(stack[bp + 2] == 0) {
        for(;bp <= sp; bp++) {
            printf("%d ", stack[bp]);
        }
    } else {
        // recursive call to previous AR on stack
        printStack(stack[bp + 2], bp - 1);
        // print the line breaker
        if(bp != (sp + 1)) printf("|");
        // print content of current AR
        for(;bp <= sp; bp++) {
            printf(" %d", stack[bp]);
        }
    }
}

void operation() {
    switch(ir.op) {
        case 1:
            sp = sp + 1;
            stack[sp] = ir.m;
            break;
        case 2:
            return ALUoperations(ir.m);
            break;
        case 3:
            sp = sp + 1;
            stack[sp] = stack[base(ir.l, bp) + ir.m];
            break;
        case 4:
            stack[base(ir.l, bp) + ir.m] = stack[sp];
            sp = sp - 1;
            break;
        case 5:
            stack[sp + 1] = 0;
            stack[sp + 2] = base(ir.l, bp);
            stack[sp + 3] = bp;
            stack[sp + 4] = pc;
            bp = sp + 1;
            pc = ir.m;
            break;
        case 6:
            sp = sp + ir.m;
            break;
        case 7:
            pc = ir.m;
            break;
        case 8:
            if(stack[sp] == 0) {
                pc = ir.m;
            }
            sp = sp - 1;
            break;
        case 9:
            if(ir.m == 0) {
                printf("%c\n", stack[sp]);
                sp = sp - 1;
            } else if(ir.m == 1) {
                scanf("%d", &stack[sp]); // read into this address
                sp = sp + 1;
            }
            break;
        default:
            break;
    }
}

int base(int level, int bp) {
    while (0 < level) {
        bp = stack[ bp + 1 ];
        level--;
    }
    return bp;
}

void ALUoperations(int operation) {
    switch(operation) {
        case 0:
            sp = bp - 1;
            pc = stack[sp + 4];
            bp = stack[sp + 3];
            break;
        case 1:
            stack[sp] = -stack[sp];
            break;
        case 2:
            sp = sp - 1;
            stack[sp] += stack[sp + 1];
            break;
        case 3:
            sp = sp - 1;
            stack[sp] -= stack[sp + 1];
            break;
        case 4:
            sp = sp - 1;
            stack[sp] *= stack[sp + 1];
            break;
        case 5:
            sp = sp - 1;
            stack[sp] /= stack[sp + 1];
            break;
        case 6:
            stack[sp] = stack[sp] % 2;
            break;
        case 7:
            sp = sp - 1;
            stack[sp] = stack[sp] % stack[sp + 1];
            break;
        case 8:
            sp = sp - 1;
            stack[sp] = (stack[sp] == stack[sp + 1]);
            break;
        case 9:
            sp = sp - 1;
            stack[sp] = (stack[sp] != stack[sp + 1]);
            break;
        case 10:
            sp = sp - 1;
            stack[sp] = (stack[sp] <  stack[sp + 1]);
            break;
        case 11:
            sp = sp - 1;
            stack[sp] = (stack[sp] <= stack[sp + 1]);
            break;
        case 12:
            sp = sp - 1;
            stack[sp] = (stack[sp] >  stack[sp + 1]);
            break;
        case 13:
            sp = sp - 1;
            stack[sp] = (stack[sp] >= stack[sp + 1]);
            break;
        default:
            break;
    }
}
