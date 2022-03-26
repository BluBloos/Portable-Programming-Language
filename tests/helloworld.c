#import <ppl/console> as pc
int main() {
    [dynamic]int myIntegers;
    for (int i = 0; i < 10; i++) {
        myIntegers.push(i);
    }
    pc::print("There are %d many integers", len(myIntegers));
}