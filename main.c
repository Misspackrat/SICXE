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
  pass1();
  pass2();
  exit(0); 
}
