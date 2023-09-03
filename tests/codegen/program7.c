b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    D ? ppl_console_print(
        "Hello: %d\n", b + c + D) :
               ppl_console_print(
        "Hellohi: %d\n", c);

    ppl_console_print("this is printed from an expression statement!\n");

    if c then ppl_console_print("c is nonzero\n");
}

d :: 0
