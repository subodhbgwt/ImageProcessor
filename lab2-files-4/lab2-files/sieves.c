#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 6
int numcount = 0;

void print_number(int n) {
  printf("%10d ", n);
  numcount++;
  if (numcount % COLUMNS == 0) {
    printf("\n"); // on all the values of numbers printed that correspond to our column number, i.e mod 6, make a new line.
  }
}

void print_sieves(int n) {
    if (n < 2) { // conditionally checks if the value even is high enough to run the funct.
        printf("\n");
        return;
    }

    unsigned char is_not_prime[n + 1]; //array that can only store positive values with a storage of n+1 bytes. All zeroes are primes, and all 1s are not.
    for (int i = 0; i <= n; i++) {
        is_not_prime[i] = 0; // set all n+1 bytes to be zero.
    }
    is_not_prime[0] = 1; // zero and one are not primes, not handled by the sieve algo.
    is_not_prime[1] = 1; // We use zero and one in the array to ensure that we can use boolean checks with !

    for (int j = 2; j * j <= n; j++) {
        if (!is_not_prime[j]) { // if j is a prime number do..
            for (int k = j * j; k <= n; k += j) {
                is_not_prime[k] = 1; // all multiples of j  are set to be 1, i.e true to being not prime.
            }
        }
    }

    for (int i = 2; i <= n; i++) {
        if (!is_not_prime[i]) {
            print_number(i);
        }
    }
    printf("\n");// a nice finishing line.
}

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
    if(argc == 2)
    {
        print_sieves (atoi(argv[1]));
    }
  else
    printf("Please state an integer number.\n");
  return 0;
}
