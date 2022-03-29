/**
 * Project 2
 * Assembler code fragment for LC-2K
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int find_label(char *);
int find_sym(char *input);

struct lab{
    char l_name[10];
    int l_addr;
    //bool added;
};

struct object{
    int header[4];
    int text[65536];
    char sym_label[65536][10];
    char symbol_table[65536][30];
    char relocation_table[65536][30];
};

struct lab label_arr[65536];
struct object overall_file;
int added_label=0;

int main(int argc, char *argv[]){
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];
    
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    
    int index=0;
    int addr=0;
    int text=0;
    int data=0;
    int sym_table=0;
    int reloc_table=0;
    int dataindex=0;

    
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if(label[0]!='\0'){
            if(find_label(label)!=-1){
                exit(1);
            }
            struct lab newlabel;
            strcpy(newlabel.l_name,label);
            newlabel.l_addr=addr;
            label_arr[index]=newlabel;
            index++;
            if(isupper(label[0])){
                char firstpart[30];
                strcpy(firstpart,label);
                if(!strcmp(opcode, ".fill")){
                    sprintf(firstpart, "%s D %d",label,data);
                } else{
                    sprintf(firstpart, "%s T %d",label,text);
                }
                strcpy(overall_file.symbol_table[sym_table],firstpart);
                sym_table++;
            }
        }
        if(!strcmp(opcode, ".fill")){
            data++;
        } else{
            text++;
        }
        addr++;
    }
    addr=0;
    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);
    int out_num=0;
    data=0;
    text=0;
    
    //labels are only be used as arg2 of I-type or arg0 of .fill
    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(!strcmp(opcode, ".fill")){
            if(isNumber(arg0)){
                out_num=atoi(arg0);
            } else{
                if(find_label(arg0)==-1){
                    if(!isupper(arg0[0])){
                        exit(1);
                    } else {
                        if(find_sym(arg0)==-1){
                            char firstpart[20];
                            sprintf(firstpart, "%s U 0",arg0);
                            strcpy(overall_file.symbol_table[sym_table],firstpart);
                            sym_table++;
                            strcpy(overall_file.sym_label[added_label],arg0);
                            added_label++;
                        }
                        out_num=0;
                    }
                } else{
                    out_num=label_arr[find_label(arg0)].l_addr;
                }
                char reloc[20];
                sprintf(reloc, "%d .fill %s",data,arg0);
                strcpy(overall_file.relocation_table[reloc_table],reloc);
                reloc_table++;
            }
            data++;
        } else if (!strcmp(opcode, "add")) {
            out_num=(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
            text++;
        } else if(!strcmp(opcode, "nor")){
            out_num=(1<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)+atoi(arg2);
            text++;
        } else if(!strcmp(opcode, "lw")){
            int offset=0;
            if(isNumber(arg2)){
                if(atoi(arg2)<-32768||atoi(arg2)>32767){
                    exit(1);
                }
                offset=atoi(arg2);
            } else{
                if(find_label(arg2)==-1){
                    if(!isupper(arg2[0])){
                        exit(1);
                    } else{
                        if(find_sym(arg2)==-1){
                            char firstpart[20];
                            sprintf(firstpart, "%s U 0",arg2);
                            strcpy(overall_file.symbol_table[sym_table],firstpart);
                            sym_table++;
                            strcpy(overall_file. sym_label[added_label],arg2);
                            added_label++;
                        }
                    }
                    offset=0;
                } else{
                    offset=label_arr[find_label(arg2)].l_addr;
                }
                char reloc[20];
                sprintf(reloc, "%d lw %s",text,arg2);
                strcpy(overall_file.relocation_table[reloc_table],reloc);
                reloc_table++;
            }
            offset=offset&65535;
            out_num=(2<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)|offset;
            text++;
        } else if(!strcmp(opcode, "sw")){
            int offset=0;
            if(isNumber(arg2)){
                if(atoi(arg2)<-32768||atoi(arg2)>32767){
                    exit(1);
                }
                offset=atoi(arg2);
            } else{
                if(find_label(arg2)==-1){
                    if(!isupper(arg2[0])){
                        exit(1);
                    } else{
                        if(find_sym(arg2)==-1){
                        char firstpart[20];
                        sprintf(firstpart, "%s U 0",arg2);
                        strcpy(overall_file.symbol_table[sym_table],firstpart);
                        sym_table++;
                        strcpy(overall_file.sym_label[added_label],arg2);
                        added_label++;
                        }
                    }
                    offset=0;
                } else{
                    offset=label_arr[find_label(arg2)].l_addr;
                }
                char reloc[20];
                sprintf(reloc, "%d sw %s",text,arg2);
                strcpy(overall_file.relocation_table[reloc_table],reloc);
                reloc_table++;
            }
            offset=offset&65535;
            out_num=(3<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16)|offset;
            text++;
        } else if(!strcmp(opcode, "beq")){
            int offset=0;
            if(isNumber(arg2)){
                if(atoi(arg2)<-32768||atoi(arg2)>32767){
                    exit(1);
                }
                offset=atoi(arg2);
            } else{
                if(find_label(arg2)==-1){
                    exit(1);
                } else{
                    offset=label_arr[find_label(arg2)].l_addr-1-addr;
                }
                if(offset<-32768||offset>32767){
                    exit(1);
                }
            }
            offset=offset&65535;
            out_num=((4<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16))|offset;
            text++;
        } else if(!strcmp(opcode, "jalr")){
            out_num=(5<<22)+(atoi(arg0)<<19)+(atoi(arg1)<<16);
            text++;
        } else if(!strcmp(opcode, "halt")){
            out_num=(6<<22);
            text++;
        } else if(!strcmp(opcode, "noop")){
            out_num=(7<<22);
            text++;
        } else{
            exit(1);
        }
        overall_file.text[dataindex]=out_num;
        dataindex++;
        addr++;
    }
    
    fprintf(outFilePtr,"%d %d %d %d\n",text,data,sym_table,reloc_table);
    for(int i=0;i<dataindex;++i){
        fprintf(outFilePtr,"%d\n",overall_file.text[i]);
    }
    for(int i=0;i<sym_table;++i){
        fprintf(outFilePtr,"%s\n",overall_file.symbol_table[i]);
    }
    for(int i=0;i<reloc_table;++i){
        fprintf(outFilePtr,"%s\n",overall_file.relocation_table[i]);
    }
    return(0);
}

int find_label(char *input){
    size_t len=sizeof(label_arr)/sizeof(label_arr[0]);
    for(int i=0;i<len;++i){
        if(!strcmp(label_arr[i].l_name, input)){
            return i;
        }
    }
    return -1;
}

int find_sym(char *input){
    for(int i=0;i<added_label;++i){
        if(!strcmp(overall_file.sym_label[i], input)){
            return i;
        }
    }
    return -1;
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
    /* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
    printf("error: line too long\n");
    exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
    /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}


