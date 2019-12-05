#include <stdio.h>

//forward declarations

void readData();
struct opcode;

int main()
{
	readData();
}

//read data method, used to read opcode.txt and make an array of opcodes
void readData()
{
	//open the file
	FILE* fptr;
	fptr = fopen("opcode.txt", "rb");

	//testing to print something out
	char ch = fgetc(fptr);
	while ((ch != '\n') && (ch != EOF))
	{
		printf("%c", ch);
		ch = fgetc(fptr);
	}
	//close the file
	fclose(fptr);

	//function psuedo code
	//while there are more lines in the file
		//read line of file
		//make a new struct
		//insert the mnuemonic
		//insert the opcode
		//insert the format
		//add the struct to an array
		//move to the next line
	//done reading data
}


//opcode.txt is formatted as mnuemonic, opcode, format
struct opcode
{
	char[6] mnuemonic;
	int opcode;
	int format;
};