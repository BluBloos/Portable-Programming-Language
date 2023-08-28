// these are compile-time constants!! :)
b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    printf("Hello, World!\n");
    return d ? b : c;
}

// the semicolon is optional for compile-time vars.
// compile-time variables also have type inference too.
d :: 9
