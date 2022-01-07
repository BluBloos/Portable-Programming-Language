#include <ppl/console>

int main() {
    [dynamic]int myIntegers;

    for (int i = 0; i < 10; i++) {
         myIntegers.append(i);
    }

    ppl::print("There are %d many integers", len(myIntegers));
}