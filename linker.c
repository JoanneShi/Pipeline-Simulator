/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	int offset;
};

struct RelocationTableEntry {
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData {
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executable
	int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry     symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};

int find_index_Glob(uint32_t globSize,char *lab,SymbolTableEntry* symbolTable){
    //int result=-1;
    for(uint32_t i=0;i<globSize;++i){
        if(!strcmp(symbolTable[i].label,lab)){
            //result=i;
            return i;
        }
    }
    return -1;
}

int find_defined_Glob(uint32_t symSize,char *lab,SymbolTableEntry* symbolTable){ //index in local symbolTable
    //int result=-1;
    for(uint32_t i=0;i<symSize;++i){
        if(!strcmp(symbolTable[i].label,lab)){
            return i;
        }
    }
    return 0;
}

bool if_U(char *lab,SymbolTableEntry* symbolTable,uint32_t size){
    for(uint32_t i=0;i<size;++i){
        if(!strcmp(symbolTable[i].label,lab)&&symbolTable[i].location=='U'){
            return true;
        } else if(!strcmp(symbolTable[i].label,lab)&&symbolTable[i].location!='U'){
            return false;
        }
    }
    return false;
}


int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES]; //300 FileData-obj array

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
    
    struct CombinedFiles allfile;
    
    uint32_t totaltext=0;
    for(i = 0; i < argc - 2; i++){
        files[i].textStartingLine=totaltext;
        totaltext+=files[i].textSize;
    }
    allfile.textSize=totaltext;
    for(i = 0; i < argc - 2; i++){
        files[i].dataStartingLine=totaltext;
        totaltext+=files[i].dataSize;
    }
    allfile.dataSize=totaltext-allfile.textSize;

    allfile.symTableSize=0;
    
    for(i = 0; i < argc - 2; i++){
        for(uint32_t m=0;m<files[i].symbolTableSize;++m){
            if(!strcmp(files[i].symbolTable[m].label,"Stack")&&files[i].symbolTable[m].location!='U'){
                //fprintf(outFilePtr, "%s\n","defined Stack");
                exit(1);
            }
            else if(files[i].symbolTable[m].location!='U'&&find_index_Glob(allfile.symTableSize, files[i].symbolTable[m].label, allfile.symTable)!=-1){
                //fprintf(outFilePtr, "%s\n",files[i].symbolTable[m].label);
                //for(int j=0;j<allfile.symTableSize;++j){
                    //fprintf(outFilePtr, "%s\n",allfile.symTable[j].label);
                //}
                //fprintf(outFilePtr, "%s\n","double defined global");
                exit(1);
            }
            if(files[i].symbolTable[m].location=='D'){
                allfile.symTable[allfile.symTableSize]=files[i].symbolTable[m];
                allfile.symTable[allfile.symTableSize].offset+=files[i].dataStartingLine;
            } else if(files[i].symbolTable[m].location=='T'){
                allfile.symTable[allfile.symTableSize]=files[i].symbolTable[m];
                allfile.symTable[allfile.symTableSize].offset+=files[i].textStartingLine;
            }
            //allfile.symTable[allfile.symTableSize]=files[i].symbolTable[m];
            //fprintf(outFilePtr, "%s %d\n","data start",files[i].dataStartingLine);
            //fprintf(outFilePtr, "%s %d\n",allfile.symTable[allfile.symTableSize].label,allfile.symTable[allfile.symTableSize].offset);
            allfile.symTableSize++;
        }
    }
    
    int ind=0;
    int index=0;
    for(i = 0; i < argc - 2; i++){
        for(j = 0; j < files[i].relocationTableSize; j++){
            if(!strcmp(files[i].relocTable[j].inst,".fill")){
                if(!strcmp(files[i].relocTable[j].label,"Stack")){
                    files[i].data[files[i].relocTable[j].offset]+=(allfile.dataSize+allfile.textSize);
                } else if(isupper(files[i].relocTable[j].label[0])){
                    ind=find_index_Glob(allfile.symTableSize,files[i].relocTable[j].label,allfile.symTable);
                    if(ind==-1){
                        //fprintf(outFilePtr, "%s\n","global label not defined");
                        exit(1);
                    }
                    if(!if_U(files[i].relocTable[j].label,files[i].symbolTable,files[i].symbolTableSize)){
                        index=find_defined_Glob(files[i].symbolTableSize, files[i].relocTable[j].label, files[i].symbolTable);
                        if(files[i].symbolTable[index].location=='D'){
                            files[i].data[files[i].relocTable[j].offset]-=(files[i].textSize+files[i].symbolTable[index].offset);
                        } else{
                            files[i].data[files[i].relocTable[j].offset]-=files[i].symbolTable[index].offset;
                        }
                    }
                    files[i].data[files[i].relocTable[j].offset]+=allfile.symTable[ind].offset;
                }
                else if((files[i].data[files[i].relocTable[j].offset]&65535)<files[i].textSize){
                    files[i].data[files[i].relocTable[j].offset]+=files[i].textStartingLine;
                } else{
                    files[i].data[files[i].relocTable[j].offset]+=files[i].dataStartingLine-files[i].textSize;
                }
            } //.fill
            else{
                if(!strcmp(files[i].relocTable[j].label,"Stack")){
                    files[i].text[files[i].relocTable[j].offset]+=(allfile.dataSize+allfile.textSize);
                } else if(isupper(files[i].relocTable[j].label[0])){
                    ind=find_index_Glob(allfile.symTableSize,files[i].relocTable[j].label,allfile.symTable);
                    if(ind==-1){
                        //fprintf(outFilePtr, "%s\n","global label not defined");
                        exit(1);
                    }
                    if(!if_U(files[i].relocTable[j].label,files[i].symbolTable,files[i].symbolTableSize)){
                        index=find_defined_Glob(files[i].symbolTableSize, files[i].relocTable[j].label, files[i].symbolTable);
                        if(files[i].symbolTable[index].location=='D'){
                            files[i].text[files[i].relocTable[j].offset]-=(files[i].textSize+files[i].symbolTable[index].offset);
                        } else{
                            files[i].text[files[i].relocTable[j].offset]-=files[i].symbolTable[index].offset;
                        }
                    }
                    files[i].text[files[i].relocTable[j].offset]+=allfile.symTable[ind].offset;
                }
                else if((files[i].text[files[i].relocTable[j].offset]&65535)<files[i].textSize){
                    files[i].text[files[i].relocTable[j].offset]+=files[i].textStartingLine;
                }else{
                    files[i].text[files[i].relocTable[j].offset]+=(files[i].dataStartingLine-files[i].textSize);
                }
            }
        }
    }
    
    
    for(i = 0; i < argc - 2; i++){
        for (j = 0; j < files[i].textSize; j++){
            fprintf(outFilePtr,"%d\n", files[i].text[j]);
        }
    }
    
    for(i = 0; i < argc - 2; i++){
        files[i].dataStartingLine=totaltext;
        for (j = 0; j < files[i].dataSize; j++){
            fprintf(outFilePtr,"%d\n", files[i].data[j]);
        }
        totaltext+=files[i].dataSize;
    }
    
} // main
