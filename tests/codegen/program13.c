b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    D ? ppl_console_print( "Hello: %d\n", b + c + D) :
        ppl_console_print( "Hellohi: %d\n", c);

    for 0 ..= 7 do
    {
        ppl_console_print("the value of it is: %d\n", it);

        if d then
        {
            ppl_console_print("d is nonzero\n");
        }
        else
        {
            ppl_console_print("d is zero\n");
        }
    }

}

d :: 0
