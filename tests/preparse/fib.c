// If we inline include like this, print() 
// is not qualified.
#import "ppl.console";

fib::(n:int)->int {
  if n <= 1
    return n;
  return fib(n - 1) + fib(n - 2);
}
main::()->int {
  print("Fib(7)=%d", fib(7));
}