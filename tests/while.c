int main() {
  int i=1; // 5 after execution
  int j=1; // 5 after execution
   while (i <= 4 || j <= 3)
   {
  	i = i + 1;
  	j = j + 1;
   }
   return i + j; // returns 10
}
