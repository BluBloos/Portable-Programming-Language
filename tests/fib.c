#include <stdio.h>

int fib(int n)
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main()
{
  printf("Fib(7)=%d", fib(7)); // should return 13
}
