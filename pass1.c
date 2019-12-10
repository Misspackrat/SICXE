//wrote mainly by Sienna Rose Sacramento
//REDid: 821626982
//CS530 Systems Programming 
//Professor Arab
//13 Decemeber 2019
//metacharacters: +, #, @ note these

#define MAX 10

struct Line
{
	char label[MAX];
	char opcode[MAX];
	char operand[MAX];
	char metaChar[MAX];
	int format = 0; 
};

void pass1()
{
	struct Line line; 
	int length = 0; //e will be 0 if parse returns a new line, -1 when EOF is found, or the length of the string if found 
	int locctr = 0; //location counter
	int progLength = 0;
	char buffer[MAX];  //update this length to something appropriate

	//open both files for writing in
	//create new intermediate file
	FILE* interFile = fopen("intermediate.txt", "w");

	//create new symtab file
	FILE* symFile = fopen("symtab.txt", "w");

	(line.label)[0] = '\0';
	(line.opcode)[0] = '\0';
	(line.operand)[0] = '\0';
	(line.metaChar)[0] = '\0';

	while (length != -1)
	{
		length = parse(buffer);
		//printf("%s is length %d\n", buffer, length);
		
		//////////////create struct to hold label, opcode, and operand//////////////////
		//search through opcode.txt to see if string matches any opcodes or if it is a label
		char str[20];
		bool op = false; 
		bool meta = false; 
		bool format4 = false;

		FILE* optxtFile = fopen("opcode.txt", "r");
		if (optxtFile == NULL)
		{
			printf("Could not open opcode.txt");
			exit(1);
		}

		while ((fgets(str, 20, optxtFile) != NULL) && (!op)) 
		{
			//get rid of newline character at end of str
			int len = strlen(str);
			str[len - 1] = 0; 

			//tokenize to get format
			int i = 0;
			char* token = strtok(str," ");
			while (token != NULL)
			{
				//printf("token %s \n", token);

				//only looking for opcodes
				if (i == 0)
				{
					int val = strcmp(buffer, str);
					//printf("buffer: %s, str: %s, val = %d\n", buffer, str,val);
					if (val == 0)
					{
						op = true;
						//printf("THEY ARE THE SAME\n");
					}
				}

				//only looking for format
				if (i == 2)
				{
					if (op)
					{
						line.format = atoi(token); 
						
					}
				}
				
				i++;
				token = strtok(NULL," ");
			}
			//printf("buffer: %s, str: %s\n", buffer, str);
		}
		fclose(optxtFile);
		
		//if buffer does match an opcode, set opcode to buffer
		//otherwise set label to buffer
		if (op)
		{
			strcpy(line.opcode, buffer);
			//printf("line.opcode %s\n", line.opcode);
		}
		else
		{
			//check if buffer is a meta character
			if (strcmp(buffer, "#") == 0)
			{
				meta = true;
				strcpy(line.metaChar, buffer);
			}
			else
				if (strcmp(buffer, "+") == 0)
				{
					meta = true;
					format4 = true;
					printf("made it to meta +\n");
					strcpy(line.metaChar, buffer);
				}
				else
					if (strcmp(buffer, "@") == 0)
					{
						meta = true;
						strcpy(line.metaChar, buffer);
					}

			//check if buffer is an operand
			bool operand = false; 
			//printf("%s line.opcode\n\n", line.opcode);
			if (!meta && ((line.opcode)[0] != '\0'))
			{
				//if buffer is an operand, save buffer into operand 
				operand = true; 
				strcpy(line.operand, buffer);
			}

			//save buffer into label if it is neither meta character or operand
				/////////////////////SYMTAB///////////////////////////
				//if label is not equal to null --> get label from struct
				  //if the label is not already in the symtab
					  //write label with address into symtab file
				 //////////////////////////////////////////////////////////
			if (!meta && !operand) 
			{
				strcpy(line.label, buffer);
				strcat(line.label, "\n");
				fputs(line.label, symFile);
			}
		}
			 
		////////////////////////////////////////////////////////////////////////////////

		//if 'START' is in opcode --> get from struct
			  //save #operand at starting address (ex: START 1000)
			  //initialize LOCCTR to starting address
		if (strcmp(line.opcode, "START") == 0)
		{
			locctr = atoi(line.operand);
		}

		//forces locctr to update only once per opcode
		if (op)
		{
			//write line to intermediate file = LOCCTR + (optional)LABEL + OPCODE + OPERAND --> get data from struct
			printf("buffer %s, opcode %s, format %d, ", buffer, line.opcode, line.format);
			printf("locctr: %d\n", locctr);

			if (format4)
			{
				locctr++;
				printf("format4 %d", locctr);
			}
	
			//update LOCCTR
			locctr += line.format; 
		}

		//reset line struct to empty all current data
		if (length == 0)
		{
			(line.label)[0] = '\0';
			(line.opcode)[0] = '\0';
			(line.operand)[0] = '\0';
			(line.metaChar)[0] = '\0';
			line.format = 0;
		}
		

		  //read next input line --> (already done with do-while loop)

	}

	//after writing the last line to the intermediate file 
	//save (LOCCTR - starting address) as program length	 

	//close files we opened
	fclose(interFile);
	fclose(symFile);

}