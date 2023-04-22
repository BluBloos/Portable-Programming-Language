odd_sum : (a:int, b:int) -> int 
{
  int h = 5;
  return h + a + b;
}

main:()->int
{
  c:int;
  a:int = 56;
  b:int = 4;
  b += a;
  return odd_sum(a, b) + c;
}
