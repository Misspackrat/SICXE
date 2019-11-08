//write out some stuff
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
int main(int argc, char *argv[])
{ 
  if((input_fd = open(argv[1],O_RDONLY))<0){ //if file does not open, send an error and exit
    perror("oops, no file opened");
    exit(1);
  }
  dup2(input_fd,STDIN_FILENO); //take input from the text file
  pass1();
  pass2();
  exit(0); 
}
