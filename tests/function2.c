odd_sum : (a:int, b:int) -> int 
{
  int h = 5;
  return h + a + b;
}

main:
{
  d:int = sizeof(char);
  a:int = 56;
  b:int = 4;
  return 2 * odd_sum(a - 1, b);
}
