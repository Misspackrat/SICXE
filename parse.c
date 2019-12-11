//parse function, takes in word by word. Returns length and updates pointer given to it
#include <stdbool.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

int parse(char* w) {
    int count = 0;
    int iochar;
    int endCount = -1;  //special case exit at -1.
    while ((iochar = getchar()) != -1) {
        if (iochar == 10) {       //10 = new line in ascii      
            *w = 0;
            if (count > 0) {      //characters are in array, unget newline to use in next call, return the count and array
                ungetc(iochar, stdin);
                return count;
            }
            else {               // nothing in array, then return null character in the array and return count 0.
                return count;
            }
        }
        if (iochar == 32) {       //32 = space in ascii
            if (count > 0) {      //space found with characters in array 
                *w = 0;         //return the array with null character at the end and the count of the characters
                return count;
            }
            else {               //space found with nothing in the array. skip over leading spaces.
                goto skipspaces;
            }
        }
        if (iochar == 64) {       //64 = @ in ascii      
            if (count > 0) {      //@ found: characters in the array, unget @ to process next call 
                *w = 0;           //set a null character at the end of array, and return count.
                ungetc(iochar, stdin);
                return count;
            }
            else {               // nothing in the character array, then return meta character in the array and return count 1.
                count++;
                *w = iochar;
                w++;
                *w = 0;
                return count;
            }
        }
        if (iochar == 44) {     //35 = # in ascii      
            if (count > 0) {      //# found: characters in the array, unget # to process next call 
                *w = 0;           //set a null character at the end of array, and return count.
                ungetc(iochar, stdin);
                return count;
            }
            else {               // nothing in the character array, then return meta character in the array and return count 1.
                count++;
                *w = iochar;
                w++;
                *w = 0;
                return count;
            }
        }
        if (iochar == 35) {     //35 = # in ascii      
            if (count > 0) {      //# found: characters in the array, unget # to process next call 
                *w = 0;           //set a null character at the end of array, and return count.
                ungetc(iochar, stdin);
                return count;
            }
            else {               // nothing in the character array, then return meta character in the array and return count 1.
                count++;
                *w = iochar;
                w++;
                *w = 0;
                return count;
            }
        }
        if (iochar == 43) {    //43 = + in ascii      
            if (count > 0) {      //# found: characters in the array, unget # to process next call 
                *w = 0;           //set a null character at the end of array, and return count.
                ungetc(iochar, stdin);
                return count;
            }
            else {                       // nothing in the character array, then return meta character in the array and return count 1.
                count++;
                *w = iochar;
                w++;
                *w = 0;
                return count;
            }
        }
        count++;
        *w = iochar;
        w++;
    skipspaces:
        ;
    }
    *w = 0;         //null character at end of string, or clears out the array for EOF etc.
    if (count > 0) {    //eof found, but still need to process any characters in array.
        ungetc(EOF, stdin);
        return count;
    }
    else {           //eof found and array is empty, return -1.
        return endCount;
    }
    return count;
}