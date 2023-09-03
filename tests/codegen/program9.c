b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    D ? ppl_console_print(
        "Hello: %d\n", b + c + D) :
               ppl_console_print(
        "Hellohi: %d\n", c);

    ppl_console_print("this is printed from an expression statement!\n");

    if d then
        ppl_console_print("d is nonzero\n");
    else
        ppl_console_print("d is zero\n");

    for 0 ..< 7 do
        ppl_console_print("how many times is this gonna print?\n");
}

d :: 0
