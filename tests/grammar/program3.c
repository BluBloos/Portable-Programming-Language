// these are compile-time constants!! :)
b :: int = 7; c :: int = 8;

main :: fn () -> s32
{
    b := 3 + b;     // this demonstrates variable shadowing.
    c := 6 * c / d; // here we use d. global scope variables can be used before their declaration.

    if ComputeDidWin(c, b)
    then
        // printf is a built-in function. in later versions of the compiler, these can only be used
        // when the appropriate header file is included.
        printf("You won! The winning numbers were (%d, %d)\n", c, b);
    else
        printf("You lost, and that sucks.\n");
}

ComputeDidWin :: fn (a : int, b : int) -> bool
{
    if a * b < 10 then
       return true;
    return false;
}

// the semicolon is optional for compile-time vars.
// compile-time variables also have type inference too.
d :: 9
