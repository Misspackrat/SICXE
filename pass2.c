#include <stdio.h>

//opcode.txt is formatted as mnuemonic, opcode, format
struct opcode
{
    char[6] mnuemonic;
    int opcode;
    int format;
};