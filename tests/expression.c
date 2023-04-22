B : struct {
  a : int;
  b : int;
  p : ^B;
};

main:()->int
{
  (54 % 8 * 9 / 10);
  A :bool= true;
  myB: B;
  myB.p[10].a = myB.b;
  return !0 + 6 == A;
}
