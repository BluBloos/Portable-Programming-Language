factorial : (n : int) -> int {
   //base case
   if (n == 0) {
      return 1;
   } else {
      return factorial(n-1) * n;
   }
}

main : ()->int {
  return factorial(3);
}
