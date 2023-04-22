main : {
  i : int = 1; // 5 after execution
  j : int = 1; // 5 after execution
  for (i <= 4 || j <= 3)
  {
    i = i + 1;
    j = j + 1;
  }
  return i + j; // returns 10
}
