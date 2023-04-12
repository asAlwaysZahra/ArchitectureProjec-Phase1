#include "assemble.h"

void main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();
    struct symbolTable *pSymTab;
    int symTabLen;

    if (argc < 3) {
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.mc will be your machine code.\n");
        exit(1);
    }
    if ((assp = fopen(argv[1], "r")) == NULL) {
        printf("%s cannot be opened\n", argv[1]);
        exit(1);
    }
    if ((machp = fopen(argv[2], "w+")) == NULL) {
        printf("%s cannot be opened\n", argv[2]);
        exit(1);
    }

    /*************************************************
     * ********************************************* *
     * ********************************************* *
     here, you can place your code for the assembler
     * ********************************************* *
     * ********************************************* *
     *************************************************/
    symTabLen = findSymTabLen(assp);
    pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++) pSymTab[i].symbol = (char *) malloc(7);
    fillSymTab(pSymTab, assp);

    for (int k = 0; k < symTabLen; ++k)
        printf("%d - %s\n", pSymTab[k].value, pSymTab[k].symbol);
    printf("--------------------------------\n");

    fclose(assp);
    fclose(machp);
}

int findSymTabLen(FILE *inputFile) {
    int count = 0;
    int lineSize = 72;
    char line[lineSize];
    while (fgets(line, lineSize, inputFile)) {
        if ((line[0] == ' ') || (line[0] == '\t') || (line[0] == '\n'));
        else {
            char *copy = (char *) malloc(sizeof(char) * 72);
            strcpy(copy, line);
            char *token = strtok(copy, "\t, ");
            if (isLabel(token)) count++;
        }
//        printf("%s", line);
    }
    rewind(inputFile);
    return count;
}

int fillSymTab(struct symbolTable *symT, FILE *inputFile) {
    int lineNo = 0;
    int lineSize = 72;
    char line[lineSize];
    int i = 0;
    while (fgets(line, lineSize, inputFile)) {
        if ((line[0] == ' ') || (line[0] == '\t') || (line[0] == '\n'));
        else {
            char *token = strtok(line, "\t, ");
            if (isLabel(token)) {
//                symT[i].symbol = (char *) malloc(8);
                strcpy(symT[i].symbol, token);
                symT[i].value = lineNo;
                i++;
            }
        }
//        printf("%d - %d - %s\n", lineNo, symT[i].value, symT[i].symbol);
        lineNo++;
    }
    rewind(inputFile);
    return lineNo;
}

int isLabel(const char *s) {
    for (int i = 0; i < 15; ++i)
        if (strcmp(instructions[i], s) == 0) return 0;
    return 1;
}

int hex2int(char *hex) {
    int result = 0;
    while ((*hex) != '\0') {
        if (('0' <= (*hex)) && ((*hex) <= '9'))
            result = result * 16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <= 'f'))
            result = result * 16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <= 'F'))
            result = result * 16 + (*hex) - 'A' + 10;
        hex++;
    }
    return (result);
}

void int2hex16(char *lower, int a) {
    sprintf(lower, "%X", a);
    if (a < 0x10) {
        lower[4] = '\0';
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x100) {
        lower[4] = '\0';
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    } else if (a < 0x1000) {
        lower[4] = '\0';
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
}
