#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void printallreg(int regist[10]);
int converttoint(int regist[10], char arg[256]);
int _add(int regist[10], int arg1, int arg2);
int _sub(int regist[10], int arg1, int arg2);
int _mul(int regist[10], int arg1, int arg2);
int _div(int regist[10], int arg1, int arg2);
int _mov(int regist[10], char arg1[256], char arg2[256]);
int _cmp(int regist[10], int arg1, int arg2);

int main(){
        // initialize (most) values
        int line = 1;
        int lines = 1;
        int preline;
        int pcounter = 0;               // pcounter is the alternative counter for "instructions executed"
        int regist[10] = {0};
        char *token;
        char inst_reg[256] = "";        // name changed since the documentation required this name.
        char copyString[256] = "";
        char curLine[4][256] = {{}};
        char infile[2049][256] = {{}};  // note I don't use line 0 for simplification
        FILE *fp;

        // read file to array
        fp = fopen("input.txt", "r");
        if(NULL == fp){
                printf("opening input.txt in the current executing directory failed!\n");
                return 1;
        }
        while(fgets(infile[lines], 256, fp)){
                infile[lines][strlen(infile[lines]) - 1] = '\0';
                lines++;
                if(lines>=2048)break;
        }
        fclose(fp);

        // now process each line
        for(int i; line <= lines; line++){
                preline = line;
                i = 0;
                curLine[0][0] = '\0';
                curLine[1][0] = '\0';
                curLine[2][0] = '\0';
                curLine[3][0] = '\0';
                if(line > 4096)return 1;                        // invalid address! Can only happen when an illegal jump has occured
                sprintf(inst_reg, "%s", infile[line]);          // workaround: only pass a copy of the current line to strtok
                strcpy(copyString, infile[line]);               // and another copy for printf later on, since [line] might change
                //printf("\n>>>curLine: %s<<<", inst_reg);        // used for debugging
                token = strtok(inst_reg, " ");
                sprintf(curLine[i], "%s", token);
                while (token != NULL && i < 4){
                        i++;
                        sprintf(curLine[i], "%s", token);
                        token = strtok(NULL, " ");
                }
                int arg1 = converttoint(regist, curLine[2]);
                int arg2 = converttoint(regist, curLine[3]);

                if(strcmp(curLine[0],"+") == 0)_add(regist, arg1, arg2);
                if(strcmp(curLine[0],"-") == 0)_sub(regist, arg1, arg2);
                if(strcmp(curLine[0],"*") == 0)_mul(regist, arg1, arg2);
                if(strcmp(curLine[0],"/") == 0)_div(regist, arg1, arg2);
                if(strcmp(curLine[0],"J") == 0)line = arg1 - 1;                                 // note: -1 because for-loop increments
                if(strcmp(curLine[0],"M") == 0)_mov(regist, curLine[2], curLine[3]);            // what could possibly go wrong here?
                if(strcmp(curLine[0],"C") == 0)_cmp(regist, arg1, arg2);
                if(strcmp(curLine[0],"B")   == 0 && regist[0] == 0)line = --arg1; 
                if(strcmp(curLine[0],"BEQ") == 0 && regist[0] == 0)line = --arg1;             // in pptx its B, in the pdf its BEQ
                if(strcmp(curLine[0],"BNQ") == 0 && regist[0] != 0)line = --arg1; 
                if(strcmp(curLine[0],"H") == 0)printf("\nHALT\n");
                if(strcmp(curLine[0],"H") == 0)exit(regist[0]);
                pcounter++;
                printf("\nRegister state after executing line %d (%s):\n", /*pcounter,*/ preline, copyString);
                printallreg(regist);
        }
        return 0;
}

void printallreg(int reg[10]){
        printf("R0: 0x%08x, R1: 0x%08x, R2: 0x%08x, R3: 0x%08x, R4: 0x%08x, R5: 0x%08x, R6: 0x%08x, R7: 0x%08x, R8: 0x%08x, R9: 0x%08x.\n",
          reg[0],     reg[1],     reg[2],     reg[3],     reg[4],     reg[5],     reg[6],     reg[7],     reg[8],     reg[9]);
  // please uncomment the lower part if the decimal expression for the registers is preferred. Above is fixed length 32 bit hex.
        //printf("R0: %d, R1: %d, R2: %d, R3: %d, R4: %d, R5: %d, R6: %d, R7: %d, R8: %d, R9: %d.\n",
        //  reg[0],     reg[1],     reg[2],     reg[3],     reg[4],     reg[5],     reg[6],     reg[7],     reg[8],     reg[9]); 
}
int converttoint(int regist[10], char arg[256]){
        if(arg[1]=='x' || arg[1] == 'X')return (int)strtol(arg, NULL, 16);      // assume hex mode
        else if(arg[0]=='r' || arg[0] == 'R')return regist[arg[1]-48];          // assume register input
        else/*(arg[0] >= '0' && arg[0] <= '9')*/ return atoi(arg);              // else try normal int
}
int _add(int regist[10], int arg1, int arg2){
        regist[0] = arg1+arg2;
        return 0;
}
int _sub(int regist[10], int arg1, int arg2){
        regist[0] = arg1-arg2;
        return 0;
}
int _mul(int regist[10], int arg1, int arg2){
        regist[0] = arg1*arg2;
        return 0;
}
int _div(int regist[10], int arg1, int arg2){
        if(arg2 == 0) return 1;         // div/0 is undefined and might cause issues
        regist[0] = arg1/arg2;
        return 0;                       
}
int _mov(int regist[10], char in1[256], char in2[256]){
        int arg1;
        int arg2;
        if(in1[0]!= 'R') return 1;
        arg1 = in1[1]-48;
        if(arg1>9 || arg1<0)return 1;   // minimalistic syntax check; 1st arg has to be a register
        if(in2[0]== 'R'){
                arg2 = in2[1]-48;
                arg2 = regist[arg2];
        }else arg2 = converttoint(regist, in2);
        regist[arg1] = arg2;
        return 0;
}
int _cmp(int regist[10], int arg1, int arg2){
        if(arg1>arg2) regist[0] = 1;
        else if(arg1<arg2) regist[0] = -1;
        else if(arg1 == arg2) regist[0] = 0;
        else return 1;
        return 0;
}