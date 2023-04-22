main: {
  sum :int= 0;
  // the below will not compile due to homogenous members.
  myArray : []int = {3; 4; 5; 6; 7 };
  for (i:int = 0 ; i < 5; i++) // the parenthesis are optional.
  {
    sum += 1;
  }
  return sum;
}
