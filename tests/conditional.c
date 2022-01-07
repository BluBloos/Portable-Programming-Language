int odd_sum(int a, int b)
{
  int h = 5;
  return h + a + b;
}

int main()
{
  const int c;
  const int a = 56 & 6;
  int b = 4 | 3;
  return odd_sum(a ? 3 : 0, b) + c;
}
