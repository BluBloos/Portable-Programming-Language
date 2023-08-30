b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    return D ? ppl_console_print(
        "Hello: %d\n", b) :
               ppl_console_print(
        "Hellohi: %d\n", c);
}

d :: 0
