//wrote mainly by Sienna Rose Sacramento
//REDid: 821626982
//CS530 Systems Programming 
//Professor Arab
//13 Decemeber 2019
//metacharacters: +, #, @ note these

struct line
{
	char[6] label;
	char[6] opcode;
	char[20] operand;
};

void pass1()
{
	int e = -1; //e will be 0 if parse returns a new line, -1 when EOF is found, or the length of the string if found 
	int locctr = 0; //location counter
	int progLength = 0;
	char d[1000];  //update this length to something appropriate

	//create new intermediate file
	//create new symtab file

	//open both files for writing in

		//begin
		  //read first input line
	do //loops through all the lines of the SICXE program until EOF
	{
		do //loops through all elements in one line of SICXE code
		{
			e = parse(d);  //every call to parse will return one word or one metacharacter

			//////////////create struct to hold label, opcode, and operand//////////////////
			//search through opcode.txt to see if string matches any opcodes or if it is a label
			//if string does match an opcode
				//set char[] label in struct to null
			//else
				//set char[] label to string
			//set opcode
			//set operand
			////////////////////////////////////////////////////////////////////////////////

		} while (e > 0);  //e will be 0 if parse returns a new line or -1 when EOF is found

		  //if 'START' is in opcode --> get from struct
			  //save #operand at starting address (ex: START 1000)
			  //initialize LOCCTR to starting address
		  //else
			  //initialize LOCCTR to 0

		  /////////////////////SYMTAB///////////////////////////
		  //if label is not equal to null --> get label from struct
			  //if the label is not already in the symtab
				  //write label with address into symtab file
		  //////////////////////////////////////////////////////////

		  //update LOCCTR

		  //write line to intermediate file = LOCCTR + (optional)LABEL + OPCODE + OPERAND --> get data from struct 
		  //read next input line --> (already done with do-while loop)
	} while (e != -1);

	//after writing the last line to the intermediate file 
	//save (LOCCTR - starting address) as program length	 

	//close files we opened

}