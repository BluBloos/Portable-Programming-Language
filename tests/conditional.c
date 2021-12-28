int odd_sum(int a, int b)
{
  int h = 5;
  return h + a + b;
}

int main()
{
  int c;
  int a = 56;
  int b = 4;
  return odd_sum(a ? 3 : 0, b) + c;
}
