/*
 prime.c
 By David Broman.
 Last modified: 2015-09-15
 This file is in the public domain.
*/

#include <stdio.h>

int is_prime(int n){
    int divcount = 0;
    if (n <= 1) {
    printf("primes are larger than one! no edge case cheesing"); // edge case
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

int main(void){
  printf("%d\n", is_prime(11));  // 11 is a prime.      Should print 1.
  printf("%d\n", is_prime(383)); // 383 is a prime.     Should print 1.
  printf("%d\n", is_prime(987)); // 987 is not a prime. Should print 0.
}
