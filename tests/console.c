#include <ppl/console>

int main() {
    ppl::print("Hello, World!\n");
    int userInput;
    ppl::print("Enter a number: ");
    ppl::scan("%d", &userInput);
    ppl::print("You entered: %d\n", userInput);
    ppl::print_color(ppl::GREEN, "This text should print green on the console\n");
    ppl::print("Should be regular color\n");

    // utf8 support and string data type.
    string utf8String = "😘"; 
    ppl::print("%s\n", utf8String);
    ppl::print_color(ppl::RED, "Angry emoji: %s\n", utf8String);
    
    // scan a utf8 string from the user.
    // this can store up to 9 characters (because the 10th character is the null terminating one)
    [10]char userString;
    ppl::print("Please enter a utf8 string: ");
    ppl::scan("%s", &userString);
    ppl::print("Here's your string back: %s\n", userString); // echo it back to the user

    // Store a single unicode character and print it.
    // in this instance char is a uint32. A single code point. 
    char unicodePoint = '🤮';

    //ppl::print("Printing a single character: %lc\n", unicodePoint);
    // triple character print test
    /*ppl::print("Printing single character three times: %c, %c, %c", 
        unicodePoint, unicodePoint, unicodePoint);*/
}