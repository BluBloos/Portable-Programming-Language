b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    return d ? ppl_console_print(
        "Hello: %d", b) :
               ppl_console_print(
        "Hellohi: %d", c);
}

d :: 0
