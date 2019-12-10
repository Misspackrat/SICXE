#include <stdio.h>
#include <stdlib.h> //for atoi()
#include <stdbool.h> //for booleans
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

//written parse function
#include "parse.h"

//forward declarations
struct opcode;
struct symbol;
struct opcode* readTableData();
struct symbol* readSymTab();
void generateObjectCode(struct opcode *table, struct symbol *syms);
void format1(int opcode, char *write);
void format2(int opcode, int reg1, int reg2, char *write);
void format3(int opcode, int flags[6], int dest, char *write);
void format4(int opcode, int flags[6], int dest, char *write);
void getFlagNum(bool n, bool i, bool x, bool b, bool p, bool e, int *nums);
struct symbol findSymbol(struct symbol *table, char *name);
struct opcode findOpcode(struct opcode *table, char *mnuemonic);
void toBinary(int decimal, char *buffer);
int toDecimal(char* binary);
void printTable(struct opcode *table);

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

int main()
{
    //pointer to opcodes
    struct opcode *cptr = readTableData();
    struct symbol *sptr = readSymTab();
    
    /*for(int i = 0; i < 3; i++)
    {   
        printf("%d. mnuemonic: %s, location: %d \n",(i+1),sptr[i].name, sptr[i].loc);
    }*/
    generateObjectCode(cptr,sptr);
}
//read table data method, used to read opcode.txt and make an array of opcodes
struct opcode* readTableData()
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
   
    if((fptr = open("opcode.txt",O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    
    char buffer[4096];
    int length = 0;

    while(count < sizeof(codes)/sizeof(codes[0]))
    {
        length = parse(buffer);
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
            if(first & !second)
            {
                int val = (int)strtol(buffer, NULL, 16);
                codes[count].opcode = val;
                second = true;
            }
            else
            {
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

struct symbol* readSymTab()
{
    //open the file
    static struct symbol syms[20];
    int fptr;
    int count = 0; 
    bool first = false;   
    bool fin = false;
    
    if((fptr = open("testsymtab.txt",O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    
    char buffer[4096];
    int length = 0;

    while(!fin)
    {
        length = parse(buffer);
        if(strcmp(buffer, "TABLEFIN") == 0)
            fin = true;
        else
        {
            if(length > 0)
            {
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
    /*for(int i = 0; i < count; i++)
    {
        printf("symbol %s is at location %d \n", syms[i].name, syms[i].loc);
    }*/
    return syms; 
}

void generateObjectCode(struct opcode *table, struct symbol *syms)
{
    //open pass 1
    int fptr;
    if((fptr = open("testpass1.txt",O_RDONLY))<0){
        perror("FILE NOT FOUND");
        exit(1);
    }
    
    dup2(fptr,STDIN_FILENO);//file opened
    FILE* fileWrite;
    fileWrite = fopen("objectcode.txt","w");
    char buffer[4096];
    char line[4096];
    int length = 0;
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
    int regA = 0;
    int regX = 0;
    int regE = 0;
    int regL = 0;
    int regB = 0;
    int regS = 0;
    int regT = 0;
    int reg1 = 0;
    int reg2 = 0;
    float regF = 0;
    int PC = 0;
    int lastPC = 0;
    int SW = 0;
    int format = 0;
    int op = 0;
    int ta = 0;
    int dest;
    
    while(length != -1)
    {
        //printf("beiningin dest is %d",dest);
        length = parse(buffer);
        //printf("%s is length %d\n",buffer, length);
        if(length == 0)
        { 
            if(!n && !i)
            {
                //printf("set simple\n");
                n = true;
                i = true;
                if(!b && !e)
                    p = true;
            }
            
            int flags[7];
            getFlagNum(n,i,x,b,p,e,flags);
            //printf("dest is %d and ta is %d\n",dest,ta);
            if(!destSet && !e)
{
                dest = ta - (PC+format);
                //printf("ping!!! dest is %d - %d\n",ta,PC);
}
            //printf("dest is %d\n",dest);
            //printf("format is %d\n", format);
            char writeBuff[16];
            switch(format)
            {
                    case 1:
                        format1(op, writeBuff);
                        break;
                    case 2:
                        format2(op,reg1,reg2, writeBuff);
                        break;
                    case 3:
                        format3(op, flags , dest, writeBuff);
                        break;
                    case 4:
                        //printf("dest is %d\n",dest);
                        format4(op, flags, dest, writeBuff);
                        break;
            }
            
            
            //write opcode to file
            fputs(line, fileWrite);
            if(format != 0){
            fputs(writeBuff, fileWrite);
            format = 0;}
            fputc('\n',fileWrite);
            
            n = false;
            i = false;
            x = false;
            b = false;
            p = false;
            e = false;

            /*printf("n is %d ", n);
            printf("i is %d ", i);
            printf("x is %d ", x);
            printf("b is %d ", b);
            printf("p is %d ", p);
            printf("e is %d\n ", e);*/
            
            dest = 0;
            ta = 0;
            destSet = false;
            first = false;
            codeNotSet = true;
            onLDB = false;
            memset(line, 0, 4096);
            //printf("cleared\n");

        }
        else
        {
            strcat(line,buffer);
            strcat(line, " ");
            //printf("line is %s \n",line);
            if(!first)
            {
                PC = (int)strtol(buffer, NULL, 16);
                //printf("pc is %d\n",PC);
                first = true;
            }
            if(codeNotSet)
            {
                struct opcode temp = findOpcode(table, buffer);
                //printf("buffer is  %s and the mnuemonic is %s\n", buffer, temp.mnuemonic);
                if(strcmp(temp.mnuemonic, "NO") != 0)
                {
                    //found a mnuemonic
                    if(format == 0)
                        format = temp.format;
                    op = temp.opcode;
                    codeNotSet = false;
                    //printf("op is %d\n",op);
                    //case specific things for each mnuemonic
                    if(strcmp(temp.mnuemonic, "TIXR") == 0)
                        reg1 = 1;
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
            
            struct symbol sym = findSymbol(syms, buffer);
           // printf("buffer is %s and syms name is %s\n",buffer, sym.name);
            if(!destSet && strcmp(sym.name, "TABLEFIN") != 0 && !codeNotSet)
            {
                //printf("the sym name is %s\n",sym.name);
                //printf("the loc is %d\n",sym.loc);
                ta = sym.loc;

                if(watchForB && ta > 4095 && !onLDB && !e)
                {
                    b = true;
                    ta = ta - regB;
                    dest = ta;
                    destSet = true;
                    //printf("base things\n");
                }
                if(e)
                {
                    destSet = true;
                    dest = ta;
                }
                //printf("the target address is %d\n", ta);
                //destSet = true;
            }

            if(setB && strcmp(sym.name, "TABLEFIN") != 0)
            {
                regB = sym.loc;
                setB = false;
            }

            //set reg numbers for format 2
            if(format == 2)
            {
                if(reg2 == 0)
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
                    if(reg1 == 0){
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
                }}

            }

            //setting flags along the way
            if(strcmp(buffer,"@") == 0)
            {
                n = true;
                //printf("indirect\n");
            }
            if(strcmp(buffer,"#") == 0)
            {
                i = true;
                lastWasI = true;
                //printf("immediate\n");
            }
            if(strcmp(buffer,"X") == 0)
            {
                x = true;
                //printf("x is found \n");
            }
            if(strcmp(buffer,"+") == 0)
            {
                e = true;
                format = 4;
                //printf("+ is found \n");
            }
        
            if(lastWasI)
            {
                if(strcmp(buffer,"#") != 0){
                struct symbol sym = findSymbol(syms, buffer);
                if(strcmp(sym.name, "TABLEFIN") != 0)
                {
                    //printf("here\n");
                    dest = sym.loc;
                }else{
                dest = (int)strtol(buffer, NULL, 16);}
                //printf("dest is immediate: %d\n",dest);
                destSet = true;
                lastWasI = false;
                }
            }   
        }
    }    
    
    fclose(fileWrite);
}

void format1(int opcode, char *write)
{
    //char str[5];
    sprintf(write, "%04x",opcode);
    //printf("format 1 object code is %s\n", str);
    //write = str;
}

void format2(int opcode, int reg1, int reg2, char *write)
{
    //char str[9];
    sprintf(write, "%02x%01x%01x", opcode, reg1, reg2);
    //printf("format 2 object code is %s\n", str);
    //write = str;
}

void format3(int opcode, int flags[6], int dest, char *write)
{   
    char opStr[32];
    toBinary(opcode, opStr);
    //printf("opcode %d is %s in binary\n", opcode, opStr);
    opStr[6] = '\0';
    //printf("opcode with last two gone is %s \n", opStr);
    for(int i = 0; i < 6; i ++)
    {
        opStr[6+i] = flags[i]+'0';
        //printf("%c",opStr[6+i]);
    }
    opStr[12] = '\0';
    //printf("opcode + flags in binary is %s \n", opStr);
    int code = toDecimal(opStr);
    //printf("converted code is %03x in hex\n", code);
    /*if(dest < 0)
        dest = dest + 0xFFFFF000;*/
    /*base =  
    while(dest < -4095)
    {
        dest = dest%base;
    }*/
    //printf("dest is %03x in hex\n", (unsigned)dest);
    //char str[9];
    if(dest < 0)
    {
    unsigned char destC[9];
    sprintf(destC, "%03x",dest);
    char *truncate = &destC[5];
    //printf("the cure is %s\n",truncate);
    sprintf(write, "%03x%s", code, truncate);
    }
    else
    sprintf(write, "%03x%03x", code, dest);
    //printf("format 3 opcode is %s\n",str);
    //write = str;
}

void format4(int opcode, int flags[6], int dest, char *write)
{
    //printf("format 4\n");
    char opStr[32];
    toBinary(opcode, opStr);
    //printf("opcode %d is %s in binary\n", opcode, opStr);
    opStr[6] = '\0';
    //printf("opcode with last two gone is %s \n", opStr);
    for(int i = 0; i < 6; i ++)
    {
        opStr[6+i] = flags[i]+'0';
        //printf("%c",opStr[6+i]);
    }
    opStr[12] = '\0';
    //printf("opcode + flags in binary is %s \n", opStr);
    int code = toDecimal(opStr);
    //printf("converted code is %03x in hex\n", code);
    //printf("dest is %05x in hex\n", dest);
    //char str[9];
    sprintf(write, "%03x%05x", code,dest);
    //printf("format 4 opcode is %s\n",str);
    //write = str;
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
struct opcode findOpcode(struct opcode *table, char *mnuemonic)
{
    for(int i = 0; i < 59; i++)
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
    
    return table[i];
}

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
    
    while(i < 8)
    {
        binary[i++] = 0;
    }
    char binStr [i];
    for(int j = i -1; j >=0; j--)
    {
        binStr[t] = binary[j] + '0';
        t++;
    }
    
    binStr[t++] = '\0';
    strncpy(buffer,binStr, t+1);
}

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

void printTable(struct opcode *table)
{
    for(int i = 0; i < 59; i++)
    {   
        printf("%d. mnuemonic: %s, opcode: %d, format:%d\n",(i+1),table[i].mnuemonic, table[i].opcode, table[i].format);
    }
}
