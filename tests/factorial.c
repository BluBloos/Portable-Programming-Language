//--------------------PREPROCESSING---------------------------
//Add #define
//Add #include

int factorial(int n) {
   //base case
   if(n == 0) {
      return 1;
   } else {
      return factorial(n-1) * n;
   }
}

int main() {
  return factorial(3);
}

//int main()
//{
  //int n = 5;
  //int i = 0;

  //printf("Factorial of %d: %d\n" , n , factorial(n));
  //printf("Fibbonacci of %d: " , n);

  //for(int i = 0; i < n; i++) {
    //printf("%d ",fibbonacci(i));
  //}

//}
