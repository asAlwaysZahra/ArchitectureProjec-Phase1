#include "assemble.h"

void main(int argc, char **argv) {
    FILE *assp, *machp, *fopen();
    struct symbolTable *pSymTab;
    int symTabLen;

    // first, check if input and output files are correct, can be opened and are readable
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
    // find number of symbols
    symTabLen = findSymTabLen(assp);
    // allocate memory for the array of symbols
    pSymTab = (struct symbolTable *) malloc(symTabLen * sizeof(struct symbolTable));
    // allocate memory for strings in symbol table
    for (int i = 0; i < symTabLen; i++) pSymTab[i].symbol = (char *) malloc(7);
    // fill symbol table with symbols and their address
    fillSymTab(pSymTab, assp);
    // print symbol table
    printf("---- symbol table ----\n");
    for (int i = 0; i < symTabLen; i++) {
        printf("      %d - %s\n", pSymTab[i].value, pSymTab[i].symbol);
    }
    printf("----------------------\n");
    // form instructions in input file and write their decimal values in output file
    formInst(assp, machp, pSymTab, symTabLen);
    // close files
    fclose(assp);
    fclose(machp);
    exit(0);
}

int findSymTabLen(FILE *inputFile) {
    int count = 0;
    int lineSize = 72;
    char line[lineSize];
    while (fgets(line, lineSize, inputFile)) {
        if ((line[0] == ' ') || (line[0] == '\t') || (line[0] == '\n') || (line[0] == '#'));
        else {
            char *copy = (char *) malloc(sizeof(char) * 72);
            strcpy(copy, line);
            char *token = strtok(copy, "\t, ");
            if (isLabel(token)) count++;
        }
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
        if ((line[0] == ' ') || (line[0] == '\t') || (line[0] == '\n') || (line[0] == '#'));
        else {
            char *token = strtok(line, "\t, ");
            if (isLabel(token)) {
//                symT[i].symbol = (char *) malloc(8);
                strcpy(symT[i].symbol, token);
                symT[i].value = lineNo;
                i++;
            }
        }
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

bool isNumeric(const char *s) {
    char c = s[0];
    for (int i = 0; c != '\0'; ++i) {
        c = s[i];
        if (c >= '0' && c <= '9' || c == '-')
            return true;
    }
    return false;
}

void formInst(FILE *input, FILE *output, struct symbolTable *pSymTab, int symTabLen) {
    int found, noInsts = 0;
    struct instruction *currInst = (struct instruction *) malloc(sizeof(struct instruction));
    int lineSize = 72, i, instCnt = 0;
    char lower[5];
    char *line = (char *) malloc(72);

    // check if a label is defined more than one
    for (i = 0; i < symTabLen - 1; i++) {
        for (int j = i + 1; j < symTabLen; j++) {
            if (strcmp(pSymTab[i].symbol, pSymTab[j].symbol) == 0) {
                fprintf(output, "Duplicate Label '%s' in lines: %d and %d\n",
                        pSymTab[i].symbol, pSymTab[i].value + 1, pSymTab[j].value + 1);
                exit(1);
            }
        }
    }

    currInst->mnemonic = (char *) malloc(7); // each mnemonic is at most 6 characters

    while (fgets(line, lineSize, input)) {
        noInsts++;

        // if line is empty
        if ((line[0] == '\n') || (line[0] == '#')) continue;

        printf("\n%d: %s", noInsts, line);
        currInst->PC = instCnt++;

        // initialize 8 parts of instruction with 0
        for (i = 0; i < 8; i++) currInst->inst[i] = '0';

        char *delim = "\t, \n";
        char *token = strtok(line, delim);

        // if first part is a label, consider the next part
        for (i = 0; i < symTabLen; i++) {
            if (strcmp(pSymTab[i].symbol, token) == 0) token = strtok(NULL, delim);
        }

        // assign mnemonic of instruction
        strcpy(currInst->mnemonic, token);

        // check .fill directive
        if (strcmp(currInst->mnemonic, ".fill") == 0) {
            token = strtok(NULL, delim);
            if (isNumeric(token)) {
                printf("%i\n", atoi(token));
                fprintf(output, "%i\n", atoi(token));
            } else {
                // find label
                found = 0;
                for (i = 0; i < symTabLen; i++) {
                    if (strcmp(pSymTab[i].symbol, token) == 0) {
                        currInst->imm = pSymTab[i].value;
                        found = 1;
                        break;
                    }
                }

                // undefined label
                if (!found) {
                    printf("\nInvalid Label '%s' in line %d\n", token, noInsts);
                    fprintf(output, "\nERROR: Invalid Label '%s' in line %d\n", token, noInsts);
                    exit(1);
                }

                // fill 8 part of instruction with hex values
                int2hex16(lower, currInst->imm);
                currInst->inst[0] = '0';
                currInst->inst[1] = '0';
                currInst->inst[2] = '0';
                currInst->inst[3] = '0';
                currInst->inst[4] = *(lower + 0);
                currInst->inst[5] = *(lower + 1);
                currInst->inst[6] = *(lower + 2);
                currInst->inst[7] = *(lower + 3);
                currInst->inst[8] = '\0';
                long result = hex2int(currInst->inst);
                printf("%li\n", result);
                fprintf(output, "%li\n", result);
            }
            continue;
        }

        // check .space directive
        if (strcmp(currInst->mnemonic, ".space") == 0) {
            int count = 0;
            while (token != NULL) {
                token = strtok(NULL, delim);
                count++;
            }
            continue;
        }

        // if the line is not a directive
        for (i = 0; i < 15; i++) {
            if (strcmp(currInst->mnemonic, instructions[i]) == 0) {
                currInst->intInst = i;
                break;
            }
        }

        // invalid instruction and op code
        if (i == 15) {
            printf("\nInvalid opcode for instruction '%s' in line %d\n", currInst->mnemonic, noInsts);
            fprintf(output, "\nERROR: Invalid opcode for instruction '%s' in line %d\n",
                    currInst->mnemonic, noInsts);
            exit(1);
        }

        currInst->instType = -1;

        // assign different parts of instruction structure based on its inst type
        if (i < 5) {
            currInst->instType = RTYPE;
            currInst->rd = atoi(strtok(NULL, delim));
            currInst->rs = atoi(strtok(NULL, delim));
            currInst->rt = atoi(strtok(NULL, delim));
            currInst->imm = 0;
        } else if (i < 13) {
            currInst->instType = ITYPE;
            currInst->rd = 0;
            currInst->rt = atoi(strtok(NULL, delim));
            if (strcmp(token, "lui") == 0)
                currInst->rs = 0;
            else
                currInst->rs = atoi(strtok(NULL, delim));

            char *token2 = strtok(NULL, delim);

            if (strcmp(token, "jalr") == 0)
                currInst->imm = 0;
            else if (isNumeric(token2))
                currInst->imm = atoi(token2);
            else {
                for (i = 0; i < symTabLen; i++) {
                    if (strcmp(pSymTab[i].symbol, token2) == 0) {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                // invalid label
                if (symTabLen == i) {
                    printf("\nInvalid Label '%s' in line %d\n", token2, noInsts);
                    fprintf(output, "\nERROR: Invalid Label '%s' in line %d\n", token2, noInsts);
                    exit(1);
                }
            }

            if (strcmp(token, "beq") == 0) {
                int temp;
                currInst->imm = currInst->imm - currInst->PC - 1;
                temp = currInst->rs;
                currInst->rs = currInst->rt;
                currInst->rt = temp;
            }

        } else {
            currInst->instType = JTYPE;
            currInst->rd = 0;
            currInst->rt = 0;
            currInst->rs = 0;
            if (strcmp(token, "j") == 0) {
                token = strtok(NULL, delim);
                // find symbol address
                for (i = 0; i < symTabLen; i++) {
                    if (strcmp(pSymTab[i].symbol, token) == 0) {
                        currInst->imm = pSymTab[i].value;
                        break;
                    }
                }
                // invalid label
                if (symTabLen == i) {
                    printf("\nInvalid Label '%s' in line %d\n", token, noInsts);
                    fprintf(output, "\nERROR: Invalid Label '%s' in line %d\n", token, noInsts);
                    exit(1);
                }
            } else
                currInst->imm = 0;
        }

        // check if offset can be placed in 16 bit
        if (currInst->imm > 32767 || currInst->imm < -32768) {
            printf("\nInvalid Offset '%d' in line %d\n", currInst->imm, noInsts);
            fprintf(output, "\nERROR: Invalid Offset '%d' in line %d\n", currInst->imm, noInsts);
            exit(1);
        }

        // hear, we convert 8 parts of instruction to 32 bit
        currInst->inst[1] = hexTable[currInst->intInst]; // bit 1 for op code in hex
        switch (currInst->instType) {
            case RTYPE:
                currInst->inst[2] = hexTable[currInst->rs];
                currInst->inst[3] = hexTable[currInst->rt];
                currInst->inst[4] = hexTable[currInst->rd];
                // other are 0 by default
                break;
            case ITYPE:
                currInst->inst[2] = hexTable[currInst->rs];
                currInst->inst[3] = hexTable[currInst->rt];
                int2hex16(lower, currInst->imm);
                currInst->inst[4] = *(lower + 0);
                currInst->inst[5] = *(lower + 1);
                currInst->inst[6] = *(lower + 2);
                currInst->inst[7] = *(lower + 3);
                break;
            default:
                int2hex16(lower, currInst->imm);
                currInst->inst[4] = *(lower + 0);
                currInst->inst[5] = *(lower + 1);
                currInst->inst[6] = *(lower + 2);
                currInst->inst[7] = *(lower + 3);
                break;
        }
        // last character must be 0
        currInst->inst[8] = '\0';
        // convert hex inst to decimal value
        long result = hex2int(currInst->inst);
        printf("%li\n", result);
        // write result in file
        fprintf(output, "%li\n", result);
    }

    fclose(input);
    free(currInst);
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
