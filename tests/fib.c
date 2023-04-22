#import "ppl.console"

fib : (n : int) -> int
{
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

main :
{
  print("Fib(7)=%d", fib(7)); // should return 13
}
