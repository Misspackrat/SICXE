#include <stdio.h>
#include <stdlib.h> //for atoi()
#include <stdbool.h> //for booleans
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#define MAXED 1048576

//written parse function
//#include "parse.h"

//forward declarations
struct opcode;
struct symbol;
struct opcode* readTableData(char *fileName);
struct symbol* readSymTab(char *fileName);
void generateObjectCode(struct opcode *table, struct symbol *syms, char *fileName);
void format1(int opcode, char *write);
void format2(int opcode, int reg1, int reg2, char *write);
void format3(int opcode, int flags[6], int dest, char *write);
void format4(int opcode, int flags[6], int dest, char *write);
void getFlagNum(bool n, bool i, bool x, bool b, bool p, bool e, int *nums);
struct symbol findSymbol(struct symbol *table, char *name);
struct opcode findOpcode(struct opcode *table, char *mnuemonic);
void toBinary(int decimal, char *buffer);
int toDecimal(char* binary);
char obCodeArray[MAXED];
int locationInArrayOb = 0; 
int counterLocOb = 0;

//opcode.txt is formatted as mnuemonic, opcode, format
struct opcode
{
    char *mnuemonic;
    int opcode;
    int format;
};

struct symbol
{
    char *name;
    int loc;
};

//my main code for testing purposes
/*int main()
{

    //pointer to opcodes
    //char file1[20];
    //strncpy("opcode.txt",file1,10);
    char *file1 = "opcode.txt";
    char *file2 = "testsymtab.txt";
    char *file3 = "testpass1.txt";
    struct opcode *cptr = readTableData(file1);
    struct symbol *sptr = readSymTab(file2);
    generateObjectCode(cptr,sptr,file3);
}*/

//read table data method, used to read opcode.txt and make an array of opcodes
struct opcode* readTableData(char *fileName)
{

    static struct opcode codes[59];
    //bools for reading
    bool first = false;
    bool second = false;

    //indicies for arrays
    int idx = 0;
    int count = 0;

    //open the file
    int fptr;
   
    if((fptr = open(fileName,O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    
    char buffer[MAXED];
    int length = 0;
    
    //go through all 59 codes
    while(count < sizeof(codes)/sizeof(codes[0]))
    {
        length = parse(buffer);
        //if its a mnuemonic
        if(!first && length != 0)
        {
            char *temp;
            temp = malloc(length+1);
            strncpy(temp,buffer,length);
            codes[count].mnuemonic = temp;
            first = true;
        }
        else
        {
            //if its and opcode
            if(first && !second)
            {
                int val = (int)strtol(buffer, NULL, 16);
                codes[count].opcode = val;
                second = true;
            }
            else
            {
                //otherwise it must be the format
                if(first && second  && length != 0)
                {
                    int val = atoi(buffer);
                    codes[count].format = val;
                    first = false;
                    second = false;
                    count++;
                }
            }
        }
    }

    return codes;
}

//this function reads in the symbol table and returns a point to it
struct symbol* readSymTab(char *fileName)
{
    //open the file
    static struct symbol syms[1024];
    int fptr;
    int count = 0; 
    bool first = false;   
    bool fin = false;
    
    //open symtab here
    if((fptr = open(fileName,O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    
    char buffer[MAXED];
    int length = 0;

    //loop til we reach the end
    while(!fin)
    {
        length = parse(buffer);
        if(strcmp(buffer, "TABLEFIN") == 0)
            fin = true;//checking for my special flag at the end
        else
        {
            if(length > 0)
            {
                //if we havent seen the first token on the line its the name
                if(!first)
                {
                    char *temp;
                    temp = malloc(length+1);
                    strncpy(temp,buffer,length);
                    syms[count].name = temp;
                    first = true;
                }
                else
                {
                    //if we have seen the first token then it must be the location
                    int val = (int)strtol(buffer, NULL, 16);
                    syms[count].loc = val;
                    first = false;
                    count++;
                }
            }
        }           
    }
    
   length = parse(buffer);
    
    syms[count].name = "TABLEFIN";
    return syms; 
}

//generates the object code from pass 1 file, saves it into objectcode.txt
void generateObjectCode(struct opcode *table, struct symbol *syms, char *fileName)
{
    //open pass 1
    int fptr;
    if((fptr = open(fileName,O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    
    //create and open file to write to    
    FILE* fileWrite;
    fileWrite = fopen("objectcode.txt","w");
    FILE* fileWriteObProg;
    fileWriteObProg = fopen("objectprogram.txt","w");
    char buffer[MAXED];
    char line[MAXED];
    char modLoc[MAXED];
    int modLocCount = 0;
    char nameProg[6];
    int length = 0;

    //pointers and counters for creating the object program.
    
    int locationInArrayPo = 0; 
    
    char lengthOfOb[MAXED]; 
    char printText[6];
    //set the first array to zero
    lengthOfOb[0] = 0;

    bool n = false;
    bool i = false;
    bool x = false;
    bool b = false;
    bool p = false;
    bool e = false;

    bool lastWasI = false;
    bool destSet = false;
    bool first = false;
    bool codeNotSet = true;
    bool watchForB = false;
    bool setB = false;
    bool onLDB = false;
    bool setF = false;
    bool reg2Set = false;
    bool reg1Set = false;
    int regB = 0;
    int reg1 = 0;
    int reg2 = 0;
    int PC = 0;//8
    int SW = 0;//9
    int format = 0;
    int op = 0;
    int ta = 0;
    int dest = 0;
    int a;
    int lineCount = 1;
    
    //start the beginning of the object program with header line:
    fputs("H", fileWriteObProg);
    
    strcpy(nameProg,syms[0].name+'\0');
    sprintf(printText, "%-6s", nameProg);
    fputs(printText, fileWriteObProg);
    sprintf(printText, "%06x", syms[0].loc);
    fputs(printText, fileWriteObProg);    
    
    //reads until end of file is found
    while(length != -1)
    {
        //get the next token
        length = parse(buffer);
        
        //at the end of a line, start processing it as a whole
        if(length == 0)
        { 
            //if not immediate or indirect addressing set the flags for simple
            if(!n && !i)
            {
                n = true;
                i = true;
                if(!b && !e)
                    p = true;
            }
            
            int flags[7];
            getFlagNum(n,i,x,b,p,e,flags);
            if(!destSet && !e)
                dest = ta - (PC+format);

            char writeBuff[1024];
            //generate object codes based on the formats
            switch(format)
            {
                    case 1:
                        format1(op, writeBuff);
                        //for(i = 0 ; i<2;i++)
                        //    obCodeArray[i+locationInArrayOb]=writeBuff[i];     
                        //locationInArrayOb=locationInArrayOb+2; 
                        break;
                    case 2:
                        format2(op,reg1,reg2, writeBuff);
                        //for(i = 0 ; i<4;i++)
                        //    obCodeArray[i+locationInArrayOb]=writeBuff[i];     
                        //locationInArrayOb=locationInArrayOb+4; 
                        break;
                    case 3:
                        format3(op, flags , dest, writeBuff);
                        //for(i = 0 ; i<6;i++)
                        //    obCodeArray[i+locationInArrayOb]=writeBuff[i];     
                        //locationInArrayOb=locationInArrayOb+6; 
                        break;
                    case 4:
                        format4(op, flags, dest, writeBuff);
                        //for(i = 0 ; i<8;i++)
                        //    obCodeArray[i+locationInArrayOb]=writeBuff[i];     
                        //locationInArrayOb=locationInArrayOb+8; 
                        modLoc[modLocCount] = (PC)+1; //for record printing
                        modLocCount++;
                        break;
            }
            
            //write lines to the file
            fputs(line, fileWrite);
            
            //object program saving pointers and counters
            if(format != 0){
                if((counterLocOb+(format))<30){
                    counterLocOb = counterLocOb + format;
                    strcpy(obCodeArray+locationInArrayOb,writeBuff);
                    locationInArrayOb= locationInArrayOb + format*2;  
                    //lengthOfOb[locationInArrayPo] = lengthOfOb[locationInArrayPo] + (format);
                    printf("%d \n",counterLocOb);
                }
                else{
                    lengthOfOb[locationInArrayPo] = counterLocOb;  
                    counterLocOb = format;
                    locationInArrayPo=locationInArrayPo+1;
                    lineCount++; 
                    obCodeArray[locationInArrayOb]='\0';
                    locationInArrayOb++;
                    strcpy(obCodeArray+locationInArrayOb,writeBuff);
                    locationInArrayOb= locationInArrayOb + format*2;  
                }
            }
              
            
            //object code was generated
            if(format != 0)
            {
                fputs(writeBuff, fileWrite);
                format = 0;
            }
            fputc('\n',fileWrite);
            
            //reset flags and values
            n = false;
            i = false;
            x = false;
            b = false;
            p = false;
            e = false;
            dest = 0;
            ta = 0;
            destSet = false;
            first = false;
            codeNotSet = true;
            onLDB = false;
            memset(line, 0, MAXED);
            


        }
        else
        {
            //keep a copy of the line as we go
            strcat(line,buffer);
            strcat(line, " ");

            //set the pc
            if(!first)
            {
                PC = (int)strtol(buffer, NULL, 16);
                first = true;
            }

            //search for which opcode to use
            if(codeNotSet)
            {
                struct opcode temp = findOpcode(table, buffer);
                if(strcmp(temp.mnuemonic, "NO") != 0)
                {
                    //found a mnuemonic
                    if(format == 0)
                        format = temp.format;
                    op = temp.opcode;
                    codeNotSet = false;

                    //case specific things for each mnuemonic
                    if(strcmp(temp.mnuemonic, "TIXR") == 0)
                    {
                        reg2 = 0;
                        reg2Set = true;
                    }

                    if(strcmp(temp.mnuemonic, "LDB") == 0)
                    {
                        watchForB = true;    
                        setB = true;
                        onLDB = true;
                    }
                    if(strcmp(temp.mnuemonic, "RSUB") == 0)
                    {   
                        destSet = true;
                        n = true;
                        i = true;
                    }
                    
                         
                }

            }
            
            //find the target address based on a location from the symbol table
            struct symbol sym = findSymbol(syms, buffer);
            if(!destSet && strcmp(sym.name, "TABLEFIN") != 0 && !codeNotSet)
            {
                ta = sym.loc;

                if(watchForB && ta > 4095 && !onLDB && !e)
                {
                    b = true;
                    ta = ta - regB;
                    dest = ta;
                    destSet = true;
                }
                if(e)
                {
                    destSet = true;
                    dest = ta;
                }
            }

            //set B register for base relative
            if(setB && strcmp(sym.name, "TABLEFIN") != 0)
            {
                regB = sym.loc;
                setB = false;
            }

            //set register numbers for format 2
            if(format == 2)
            {
                if(!reg2Set)
                {
                    if(buffer[0] == 'A')
                        reg2 = 0;
                    if(buffer[0] == 'X')
                        reg2 = 1;
                    if(buffer[0] == 'L')
                        reg2 = 2;
                    if(buffer[0] == 'B')
                        reg2 = 3;
                    if(buffer[0] == 'S')
                        reg2 = 4;
                    if(buffer[0] == 'T')
                        reg2 = 5;
                    if(buffer[0] == 'F')
                        reg2 = 6;
                    if(strcmp(buffer,"PC") == 0 || strcmp(buffer,"PC,") == 0 )
                        reg2 = 8;
                    if(strcmp(buffer,"SW") == 0 || strcmp(buffer,"SW,") == 0 )
                        reg2 = 9;
                }
                else
                {
                    if(!reg1Set){
                        if(buffer[0] == 'A')
                            reg1 = 0;
                        if(buffer[0] == 'X')
                            reg1 = 1;
                        if(buffer[0] == 'L')
                            reg1 = 2;
                        if(buffer[0] == 'B')
                            reg1 = 3;
                        if(buffer[0] == 'S')
                            reg1 = 4;
                        if(buffer[0] == 'T')
                            reg1 = 5;
                        if(buffer[0] == 'F')
                            reg1 = 6;
                        if(strcmp(buffer,"PC") == 0 || strcmp(buffer,",PC") == 0 )
                            reg1 = 8;
                        if(strcmp(buffer,"SW") == 0 || strcmp(buffer,",SW") == 0 )
                            reg1 = 9;
                        
                        reg1Set = true;
                    }
                }
            }

            //setting flags along the way
            if(strcmp(buffer,"@") == 0)
            {
                n = true;
            }
            if(strcmp(buffer,"#") == 0)
            {
                i = true;
                lastWasI = true;
            }
            if(strcmp(buffer,"X") == 0)
            {
                x = true;
            }
            if(strcmp(buffer,"+") == 0)
            {
                e = true;
                format = 4;
            }
            
            //take care of immediate addressing
            if(lastWasI)
            {
                if(strcmp(buffer,"#") != 0){
                struct symbol sym = findSymbol(syms, buffer);
                if(strcmp(sym.name, "TABLEFIN") != 0)
                {
                    dest = sym.loc;
                }
                else
                {
                    dest = (int)strtol(buffer, NULL, 16);}
                    destSet = true;
                    lastWasI = false;
                }
            }   
        }
    }    
    
    //finish the first line in the object program aka the header line
    sprintf(printText, "%06x", PC);
    fputs(printText, fileWriteObProg);
    fputc('\n', fileWriteObProg);
    //place the text part into the object program, each line only reaching 30 or less
    int y;
    int t;
    int z = syms[0].loc;
    int c = 0;
    locationInArrayOb = 0; //reset to zero
    for(y = 0; y < lineCount; y++){
        fputs("T", fileWriteObProg);
        sprintf(printText, "%06x", z); //location starting on this line in the text record.
        fputs(printText, fileWriteObProg);
        sprintf(printText, "%02x", lengthOfOb[y]);  //length of this text record line.
        fputs(printText, fileWriteObProg);
        fputs(obCodeArray+c, fileWriteObProg);
        //locationInArrayOb=locationInArrayOb+lengthOfOb[y*2]+1;
        fputc('\n', fileWriteObProg);
        z = z + lengthOfOb[y];
        c = (c + lengthOfOb[y])*2 +1;
    }
    
    
    for(y = 0; y < modLocCount; y++){
        fputs("M", fileWriteObProg);
        sprintf(printText, "%06x", modLoc[y]);
        fputs(printText, fileWriteObProg);
        sprintf(printText, "%02x", 05);
        fputs(printText, fileWriteObProg);
        fputc('\n', fileWriteObProg);
    }


    //place the end of record in the object program
    fputs("E", fileWriteObProg);
    sprintf(printText, "%06x", syms[0].loc);
    fputs(printText, fileWriteObProg);
    
    fclose(fileWriteObProg);//close file for object program
    fclose(fileWrite);//close file for objectcode
}

//single byte object code converter (returns into char pointer)
void format1(int opcode, char *write)
{
    sprintf(write, "%04x",opcode+'\0');
}

//2 byte object code converter (returns into char pointer)
void format2(int opcode, int reg1, int reg2, char *write)
{
    sprintf(write, "%02x%01x%01x", opcode, reg1, reg2+'\0');
}

//3 byte object code converter (returns into char pointer)
void format3(int opcode, int flags[6], int dest, char *write)
{   
    char opStr[32];
    toBinary(opcode, opStr);
    opStr[6] = '\0';
    int i;

    for(i = 0; i < 6; i ++)
    {
        opStr[6+i] = flags[i]+'0';
    }
    opStr[12] = '\0';

    int code = toDecimal(opStr);
    //if the dest is negative get rid of pesky leading zeros
    if(dest < 0)
    {
        unsigned char destC[9];
        sprintf(destC, "%03x",dest);
        char *truncate = &destC[5];
        sprintf(write, "%03x%s", code, truncate);
    }
    else
        sprintf(write, "%03x%03x", code, dest+'\0');
}

//extended format object code converter (returns into char pointer)
void format4(int opcode, int flags[6], int dest, char *write)
{
    char opStr[32];
    toBinary(opcode, opStr);
    opStr[6] = '\0';
    int i;

    for(i = 0; i < 6; i ++)
    {
        opStr[6+i] = flags[i]+'0';
    }

    opStr[12] = '\0';
    int code = toDecimal(opStr);
    sprintf(write, "%03x%05x", code,dest+'\0');
}

//sets an array of ones and zeros representing the flags
void getFlagNum(bool n, bool i, bool x, bool b, bool p, bool e, int *nums)
{
    if(n)
        nums[0] = 1;
    else
        nums[0] = 0;
    if(i)
        nums[1] = 1;
    else
        nums[1] = 0;
    if(x)
        nums[2] = 1;
    else
        nums[2] = 0;
    if(b)
        nums[3] = 1;
    else
        nums[3] = 0;
    if(p)
        nums[4] = 1;
    else
        nums[4] = 0;
    if(e)
        nums[5] = 1;
    else
        nums[5] = 0;
}

//searches through the opcodes to get information about the mneumonic
struct opcode findOpcode(struct opcode *table, char *mnuemonic)
{
    int i;
    for(i = 0; i < 59; i++)
    {
        if(strcmp(mnuemonic, table[i].mnuemonic) == 0)
        {
            return table[i];
        }
    }
    
    //case its not an opcode
    struct opcode no;
    no.mnuemonic = "NO";
    return no;
}

//searches through the symbol table and checks if the given name is in there
struct symbol findSymbol(struct symbol *table, char *name)
{
    int i = 0;
    while(strcmp(table[i].name, "TABLEFIN") != 0)
    {
        if(strcmp(name, table[i].name) == 0)
        {
            return table[i];
        }
        i++;
    }
    //returns tablefin (my special flag)
    return table[i];
}

//converts a decimal number into a binary string
void toBinary(int decimal, char* buffer)
{
    int binary[32];
    int d = decimal;
    int i = 0;
    int t = 0;
    
    while(decimal > 0)
    {
        binary[i] = decimal%2;
        decimal = decimal/2;
        i++;
    }
    
    //add leadin zeros
    while(i < 8)
    {
        binary[i++] = 0;
    }
    
    //reverse the numbers
    char binStr [i];
    int j;
    for(j = i -1; j >=0; j--)
    {
        binStr[t] = binary[j] + '0'; 
        t++;
    }
    
    binStr[t++] = '\0';
    strncpy(buffer,binStr, t+1);
}

//converts a binary string into decimal form
int toDecimal(char* binary)
{
    long num = atol(binary);
    int decimal = 0;
    long temp = num;
    int base = 1;
    //printf("num is %ld", num);
    
    while(temp)
    {
        int last = temp%10;
        temp = temp/10;
        decimal += last*base;
        base = base*2;
    }

    return decimal;
}
