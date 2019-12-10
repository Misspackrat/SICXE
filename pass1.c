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
	bool format4 = false;

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
		bool hasInput = true; 

		if (length == 0)
			hasInput = false;

		FILE* optxtFile = fopen("opcode.txt", "r");
		if (optxtFile == NULL)
		{
			printf("Could not open opcode.txt");
			exit(1);
		}

		FILE* keywordFile = fopen("keywords.txt", "r");
		if (keywordFile == NULL)
		{
			printf("Could not open keywords.txt");
			exit(1);
		}

		while ((fgets(str, 20, optxtFile) != NULL) && (!op) && hasInput) 
		{
			//get rid of newline character at end of str
			int len = strlen(str);
			str[len - 1] = 0; 

			//tokenize to get format
			int i = 0;
			char* token = strtok(str," ");
			while (token != NULL)
			{
				//if buffer does match an opcode, set opcode to buffer
				if (i == 0)
				{
					int val = strcmp(buffer, str);
					if (val == 0)
					{
						op = true;
						strcpy(line.opcode, buffer);
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

			//looking for keywords
			while ((fgets(str, 20, keywordFile) != NULL) && (!op) && hasInput)
			{
				//get rid of newline character at end of str
				int len = strlen(str);
				str[len - 1] = 0;

				//tokenize to get format
				char* token = strtok(str, " ");
				while (token != NULL)
				{
					
					//if buffer does match an opcode, set opcode to buffer
					int val = strcmp(buffer, str);
					if (val == 0)
					{
						op = true;
						strcpy(line.opcode, buffer);
					}
					
					token = strtok(NULL, " ");
				}
			}
		}
		fclose(optxtFile);
		fclose(keywordFile);
		
		if(!op && hasInput)
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
			if (!meta && hasInput && ((line.opcode)[0] != '\0'))
			{
				//if buffer is an operand, save buffer into operand 
				operand = true; 
				strcpy(line.operand, buffer);
				//printf("%s line.operand\n\n", line.operand);
			}

			//save buffer into label if it is neither meta character or operand
				/////////////////////SYMTAB///////////////////////////
				//if label is not equal to null --> get label from struct
				  //if the label is not already in the symtab
					  //write label with address into symtab file
				 //////////////////////////////////////////////////////////
			if (!meta && !operand && hasInput && (strlen(buffer) > 0))
			{
				char out[MAX]; 
				char temp[MAX];

				strcpy(line.label, buffer);
				strcpy(out, line.label);
				strcat(out, " ");
				strcat(out, itoa(locctr, temp, 16));
				strcat(out, "\n");
				fputs(out, symFile);
				//int len = strlen(line.label);
				//(line.label)[len - 1] = 0;
			}
		}

		if (!hasInput)
		{
			char output[25];
			bool base = false; 

			//if 'START' is in opcode --> get from struct
			//save #operand at starting address (ex: START 1000)
			//initialize LOCCTR to starting address
			if (strcmp(line.opcode, "START") == 0)
			{
				locctr = atoi(line.operand);
			}

			//checking if opcode is equal to BASE,
			//if it is then doent print location
			if ((strcmp(line.opcode, "BASE") == 0) || strcmp(line.opcode, "END") == 0)
			{
				base = true; 
				strcpy(output, line.opcode);
				strcat(output, " ");
				strcat(output, line.operand);
			}
			else
			{
				itoa(locctr,output,16);
			}
			
			strcat(output, " ");
			if ((strlen(line.label) > 0) && !base)
			{
				strcat(output, line.label);
				strcat(output, " ");
			}
			if ((strlen(line.metaChar) > 0) && !base)
			{
				strcat(output, line.metaChar);
			}
			
			if (!base)
			{
				strcat(output, line.opcode);
				strcat(output, " ");
			}
			
			if ((strlen(line.operand) > 0) && !base) 
			{
				strcat(output, line.operand);
				
			}
			strcat(output, "\n");

			fputs(output, interFile);
			//printf("buffer %s\nlabel %s, opcode %s, format %d, meta %s, operand %s, ", buffer, line.label, line.opcode, line.format, line.metaChar, line.operand);
			//printf("locctr: %d\n", locctr);
			//printf("length %d\n\n", length);
		}
		

		if (length == 0) 
		{
			////////////////////////////////////////////////////////////////////////////////

			if (strcmp(line.opcode, "RESW") == 0)
			{
				locctr += (3 * atoi(line.operand));
			}

			if (strcmp(line.opcode, "RESB") == 0)
			{
				locctr += atoi(line.operand);
			}

			//write line to intermediate file = LOCCTR + (optional)LABEL + OPCODE + OPERAND --> get data from struct
			//printf("buffer %s, opcode %s, format %d, meta %s, operand %s, ", buffer, line.opcode, line.format, line.metaChar, line.operand);
			//printf("locctr: %d\n", locctr);

			//update LOCCTR
			locctr += line.format;

			if (format4)
			{
				locctr++;
				format4 = false; 
			}

			//reset line struct to empty all current data
			(line.label)[0] = '\0';
			(line.opcode)[0] = '\0';
			(line.operand)[0] = '\0';
			(line.metaChar)[0] = '\0';
			line.format = 0;
		}
	}

	//after writing the last line to the intermediate file 
	//save (LOCCTR - starting address) as program length	

	fputs("TABLEFIN", symFile);

	//close files we opened
	fclose(interFile);
	fclose(symFile);

}