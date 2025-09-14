/*
 print-primes.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/


#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 6
int numcount = 0; // numcount for printnum is here to ensure its not reset every time the call is run. Couldve also made it static in func.

int is_prime(int n){
    int divcount = 0;
    if (n <= 1) {
    return 0; // edge case for negative values and 1.
    } else {
      for (int i = 1; i <= n; i++) {
        if (n % i == 0) { // if n is divisible by any number below itself apart from 1 and itself, its not a prime. we use modulus for this. rest zero implies its perfectly divisible.
          divcount++;
        }
      }
      if (divcount > 2) { // if its not a prime, return zero.
        return 0;
      } 
      return 1; // base case, always prints one unless its NOT true.
  }
}

void print_number(int n) {
  printf("%10d ", n);
  numcount++;
  if (numcount % COLUMNS == 0) {
    printf("\n"); // on all the values of numbers printed that correspond to our column number, i.e mod 6, make a new line.
  }
}


void print_primes(int n){
    // Should print out all prime numbers less than and up to 'n'
    // with the following formatting. Note that
    // the number of columns is stated in the define
    // COLUMNS
    for (int i = 2; i <= n; i++) { // instantiate i at 2 here because 1 isnt a prime. but is only divisible by itself, so it technically fulfills the criteria.
      if (is_prime(i)) {
        print_number(i);
      }
    }
}

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
    if(argc == 2)
    {
        print_primes(atoi(argv[1]));
    }
  else
    printf("Please state an integer number.\n");
  return 0;
}

 
