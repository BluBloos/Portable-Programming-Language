// If we inline include like this, print() 
// is not qualified.
#include <ppl.console>
int fib(int n) {
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}
int main() {
  print("Fib(7)=%d", fib(7));
}