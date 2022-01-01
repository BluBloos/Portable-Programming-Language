#include <ppl/console>

int main() {
    ppl::print("Hello, World!\n");
    int userInput;
    ppl::print("Enter a number: ");
    ppl::scan("%d", &userInput);
    ppl::print("You entered: %d\n", userInput);
    ppl::print_color(ppl::GREEN, "This text should print green on the console\n");
    ppl::print("Should be regular color\n");
}