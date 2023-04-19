
myIntegers : [10]int; // = {}

// three statements here are allowed.
// the first is the init statement. the second is the loop exit condition. the third is the loop postamble.
for i := 0u; i < 10; i++ {
    myIntegers[i]=i;
}

// but we can also have just two statements.
// where the first is the loop exit condition and the second is the postamble.
i:=0u;
for i < 10; i++ {
    myIntegers[i]=i;
}

// and finally we can have just one statement.
// where the thing is the loop exit condition.
i:=0u;
for i < 10 {
    myIntegers[i]=i;
    i++;
}

// the way this is done is clear. there is no need for a while loop!