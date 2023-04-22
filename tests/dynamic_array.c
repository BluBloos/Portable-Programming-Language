pc : #import "ppl.console"
#import "ppl.array"

main:()->int {
    myIntegers : array<int>;

    for i : int = 0; i < 10; i++ {
        myIntegers.push(i);
    }

    pc.print("There are %d many integers", myIntegers.size);
}