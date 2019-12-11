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
	int format;
};

void pass1()
{
	struct Line line;
	int length = 0; //e will be 0 if parse returns a new line, -1 when EOF is found, or the length of the string if found 
	int locctr = 0; //location counter
	int progLength = 0;
	char buffer[MAX];  //update this length to something appropriate
	bool format4 = false;
	int metaType = 0;

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

		//////////////create struct to hold label, opcode, and operand//////////////////
		//search through opcode.txt to see if string matches any opcodes or if it is a label
		char str[20];
		bool op = false;		//true when there is an opcode
		bool meta = false;		//true when line of assembley 
		bool hasInput = true;	//true when buffer is not a newline or EOF character

		if (length == 0)
			hasInput = false;

		//opens and reads from opcode.txt to see if buffer is a opcode
		FILE* optxtFile = fopen("opcode.txt", "r");
		if (optxtFile == NULL)
		{
			printf("Could not open opcode.txt");
			exit(1);
		}

		//opens and reads from keywords.txt to see if buffer is a keyword (i.e. not an operand but a special command)
		FILE* keywordFile = fopen("keywords.txt", "r");
		if (keywordFile == NULL)
		{
			printf("Could not open keywords.txt");
			exit(1);
		}

		//reads from opcode.txt line by line
		while ((fgets(str, 20, optxtFile) != NULL) && (!op) && hasInput)
		{
			//get rid of newline character at end of str
			int len = strlen(str);
			str[len - 1] = 0;

			//tokenize to get format
			int i = 0;
			char* token = strtok(str, " ");
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

				//only looking for format type
				if (i == 2)
				{
					if (op)
					{
						line.format = atoi(token);
					}
				}
				i++;
				token = strtok(NULL, " ");
			}

			//looking for keywords by reading line by line from keywords.txt
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

		//close files
		fclose(optxtFile);
		fclose(keywordFile);

		if (!op && hasInput)
		{
			//check if buffer is a meta character
			if (strcmp(buffer, "#") == 0)
			{
				meta = true;
				metaType++;
				strcpy(line.metaChar, buffer);
			}
			else
				if (strcmp(buffer, "+") == 0)
				{
					meta = true;
					format4 = true;
					metaType++;
					strcpy(line.metaChar, buffer);
				}
				else
					if (strcmp(buffer, "@") == 0)
					{
						meta = true;
						metaType++;
						strcpy(line.metaChar, buffer);
					}

			//check if buffer is an operand
			bool operand = false;
			if (!meta && hasInput && ((line.opcode)[0] != '\0'))
			{
				//if buffer is an operand, save buffer into operand
				if (strlen(line.operand) > 0)
				{
					operand = true;
					strcat(line.operand, buffer);
				}
				else
				{
					operand = true;
					strcpy(line.operand, buffer);
				}
				
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
				char buff[32];
				sprintf(buff, "%06x", locctr);
				strcat(out, buff);
				//strcat(out, itoa(locctr, temp, 16));
				strcat(out, "\n");
				//printf("%s\n",out);
				fputs(out, symFile);
				//int len = strlen(line.label);
				//(line.label)[len - 1] = 0;
			}
		}

		//buffer is a newline or EOF we want to update locctr and save data into intermediate file
		if (!hasInput)
		{
			char output[25];
			bool base = false;
			bool metaOnOp = false;

			//if 'START' is in opcode --> get from struct
			//save #operand at starting address (ex: START 1000)
			//initialize LOCCTR to starting address
			if (strcmp(line.opcode, "START") == 0)
			{
				locctr = atoi(line.operand);
			}

			//checking if opcode is equal to BASE,
			//if it is then don't print location
			if ((strcmp(line.opcode, "BASE") == 0))
			{
				base = true;
				strcpy(output, line.opcode);
				strcat(output, " ");
				strcat(output, line.operand);
			}
			else
			{
				//save locctr
				//char bu[32];
				sprintf(output, "%04x", locctr);
				//strcat(output, bu);

				//itoa(locctr,output,16);
				//printf("%s \n", output);
			}

			//save label if there is one
			strcat(output, " ");
			if ((strlen(line.label) > 0) && !base)
			{
				strcat(output, line.label);
				strcat(output, " ");
			}

			//save meta char is there are any
			if ((strlen(line.metaChar) > 0) && !base)
			{
				//just a +
				if (format4 && (metaType == 1))
				{
					strcat(output, "+");
				}

				//more than 2 meta characters
				if (metaType > 1)
				{
					strcat(output, "+");
					metaOnOp = true;
				}

				//just one meta char but it attaches to the operand
				if (!format4 && (metaType == 1))
				{
					metaOnOp = true;
				}


			}

			//save opcode 
			if (!base)
			{
				strcat(output, line.opcode);
				strcat(output, " ");
			}

			//save operand
			if ((strlen(line.operand) > 0) && !base)
			{
				//attach meta character to operand
				if (metaOnOp)
				{
					strcat(output, line.metaChar);
				}
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
       
			//checking for keywords that need addresses

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

			//increase locctr to accomidate format4
			if (format4)
			{
				locctr++;
				format4 = false;
			}
      

			//reset meta character count
			metaType = 0;

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
	char output[25];
	sprintf(output, "%04x", locctr);
	fputs(output, interFile);
	fputs("TABLEFIN", symFile);

	//close files we opened
	fclose(interFile);
	fclose(symFile);

}
