b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 8u;
    return D ? ppl_console_print(
        "Hello: %d\n", b + c + D) :
               ppl_console_print(
        "Hellohi: %d\n", c);
}

d :: 0
