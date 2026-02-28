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

void copycodes(const char *a0, int *a1, int *counter) {
  int t0;
  int t1;
  while (1) { 
    t0 = (int)*a0; // get character from the string lb t0, 0(a0)
    if (t0 == 0) break; // if it is zero, we are done, i.e null as the last character.
    *a1 = t0; // store the value of t0 into a1. sw t0, 0(a1)

    a0++; // increment the pointer to the string. addi a0, a0, 1
    a1++; // increment the pointer to the list. addi a1, a1, 4, each int is 4 bytes.

    t1 = *counter; // load counter into t1. lw t1, 0(a2)
    t1++; // increment t1. addi t1, t1, 1
    *counter = t1; // store t1 back into counter. sw t1, 0(a2)
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
