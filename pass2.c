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
struct opcode* readTableData();
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

int main()
{
    //pointer to opcodes
    struct opcode *cptr = readTableData();
    //printTable(cptr);
    //printf("ADDF has the format of %d\n", findOpcode(cptr,"ADDF").format);
    char bin[32];
    toBinary(6,bin);
    toDecimal(bin);
}
//read table data method, used to read opcode.txt and make an array of opcodes
struct opcode* readTableData( )
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
            //printf("storing %s\n",temp);
            codes[count].mnuemonic = temp;
            first = true;
        }
        else
        {
            if(first & !second)
            {
                //printf("%s is ", buffer);
                int val = (int)strtol(buffer, NULL, 16);//atoi(buffer);
                //printf("%d \n",val);
                codes[count].opcode = val;
                second = true;
            }
            else
            {
                if(first && second  && length != 0)
                {
                    //printf("%s is ", buffer);
                    int val = atoi(buffer);
                    //printf("%d \n",val);
                    codes[count].format = val;
                    first = false;
                    second = false;
                    count++;
                }
            }
        }
    }

    //printTable(codes);
    //c = codes;

    return codes;
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
    return table[0];
}

void toBinary(int decimal, char* buffer)
{
    int binary[32];
    int d = decimal;
    int i = 0;
    while(decimal > 0)
    {
        binary[i] = decimal%2;
        decimal = decimal/2;
        i++;
        //printf("ping\n");
    }
    
    char binStr [i];
    int t = 0;
    //printf("i is %d j is % d\n", i, i-1);
    for(int j = i -1; j >=0; j--)
    {
        binStr[t] = binary[j] +'0';
        t++;
        //printf("ping");
    }
    binStr[t++] = '\0';
    //printf("%s is binstr\n",binStr);
    strncpy(buffer,binStr, t+1);
   // printf("%s is buffer\n",buffer);
    //buffer[i] = '\0';
    //printf("%d is %s in binary\n", d, binStr);
    /*char * bptr = new char;
    return bptr;*/
}

int toDecimal(char* binary)
{
    int num = atoi(binary);
    int decimal = 0;
    //printf("%s is %d\n",binary,num);
    int temp = num;
    int base = 1;
    while(temp)
    {
        int last = temp%10;
        temp = temp/10;
        decimal += last*base;
        base = base*2;
    }
    //printf("decimal for %s is %d\n",binary,decimal);
    return decimal;
}

void printTable(struct opcode *table)
{
    for(int i = 0; i < 59; i++)
    {   
        printf("%d. mnuemonic: %s, opcode: %d, format:%d\n",(i+1),table[i].mnuemonic, table[i].opcode, table[i].format);
    }
}
