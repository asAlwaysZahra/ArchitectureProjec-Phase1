#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NORTYPE 5
#define NOITYPE 8
#define NOJTYPE 2
#define RTYPE 0
#define ITYPE 1
#define JTYPE 2

/* this structure is defined to hold ever entity of symbol table    *
 * which, after finding the length, will become an array of symbols */
struct symbolTable {
    int value;
    char *symbol;
};

struct instruction {
    size_t instType; // 0 means r type, 1 means i type and 2 means j type
    size_t intInst;
    char *mnemonic;
    char inst[9];
    int rs;
    int rt;
    int rd;
    int imm;
    int PC;
};

int isLabel(const char *);

char *instructions[] = {"add", "sub", "slt", "or", "nand",
                        "addi", "slti", "ori", "lui", "lw", "sw", "beq", "jalr",
                        "j", "halt"};

int findSymTabLen(FILE *);

int fillSymTab(struct symbolTable *, FILE *);

void formInst(struct instruction *, FILE *);

int hex2int(char *);

void int2hex16(char *, int);
