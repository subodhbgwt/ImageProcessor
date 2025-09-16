/*
 pointers.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/


#include <stdio.h>
#include <stdlib.h>

char* text1 = "This is a string.";
char* text2 = "Yet another thing.";

int list1[20]; // instantiate two lists with integers in them. size is 20 because 80 bytes corresponds to 20 ints.
int list2[20];
int counter = 0;

void copycodes(const char *src, int *dst, int *cnt) { // a0 = src char, a1 = dst int, a2 = &counter
  while (*src != 0) { // as long as the pointer doesn't point to an integer zero, we continue. Also works with '\0' or just *src.
    unsigned char ch = (unsigned char)*src; // corresponds to lb = to, 0(a0)
    *dst = (int)ch; // sw = t0, 0(a1)
    src++; // corresponds to addi a0, a0, 1 and a1, a1, 4
    dst++;

    (*cnt)++;
  }
}
 // text one goes into list 1, text 2 goes into list 2.
void work(void) {
  copycodes(text1, list1, &counter);
  copycodes(text2, list2, &counter);
}



void printlist(const int* lst){
  printf("ASCII codes and corresponding characters.\n");
  while(*lst != 0){
    printf("0x%03X '%c' ", *lst, (char)*lst);
    lst++;
  }
  printf("\n");
}

void endian_proof(const char* c){
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n", 
         (int)*c,(int)*(c+1), (int)*(c+2), (int)*(c+3));
  
}

int main(void){
 
    work();
    printf("\nlist1: ");
    printlist(list1);
    printf("\nlist2: ");
    printlist(list2);
    printf("\nCount = %d\n", counter);

    endian_proof((char*) &counter);
}
