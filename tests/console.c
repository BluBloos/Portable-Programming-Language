#import "ppl.console";

main : () -> int {

    print("Hello, World!\n");

    userInput : int;
    print("Enter a number: ");
    scan("%d", &userInput);
    print("You entered: %d\n", userInput);

    // ppl.console.color.GREEN
    print_color(color.GREEN, "This text should print green on the console\n");

    print("Should be regular color\n");

    // utf8 support and string data type.
#if 0
    // TODO:
    utf8String : string = "😘😘_!";
    print("%s\n", utf8String);
    print_color(color.RED, "Angry emoji: %s\n", utf8String);
#endif

    // scan an ASCII string from the user.
    // this can store up to 9 characters (because the 10th character is the null terminating one).
    userString : [10]char;

    print("Please enter a utf8 string: ");
    scan("%s", &userString);
    print("Here's your string back: %s\n", userString); // echo it back to the user

    // Store a single unicode character and print it.
    // in this instance char is a uint32. A single code point. 
    unicodePoint : ui32 = '🤮';

    // TODO: is there a way to print a single unicode character or must it be done
    // as a string?
}