odd_sum:(a:int, b:int)->int 
{
  h : int = 5;
  return h + a + b;
}

main:()->int
{
  // TODO: hook const.
  c : const int;
  a : const int = 56 & 6;
  b : int = 4 | 3;
  return odd_sum(a ? 3 : 0, b) + c;
}
