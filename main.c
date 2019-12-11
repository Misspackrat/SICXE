//write out some stuff
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/wait.h>
#include <signal.h>

#include "parse.h"
#include "pass1.c"
#include "pass2.c"


int main(int argc, char *argv[])
{ 
    int input_fd; 

  if((input_fd = open(argv[1],O_RDONLY))<0){ //if file does not open, send an error and exit
    perror("oops, no file opened");
    exit(1);
  }
  dup2(input_fd,STDIN_FILENO); //take input from the text file

  pass1();
  char *file1 = "opcode.txt";
  char *file2 = "symtab.txt";
  char *file3 = "intermediate.txt";
  struct opcode *cptr = readTableData(file1);
  struct symbol *sptr = readSymTab(file2);
  generateObjectCode(cptr,sptr,file3); 
  //pass2();
  exit(0); 
}
