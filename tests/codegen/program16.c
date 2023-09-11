b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    D ? ppl_console_print( "Hello: %d\n", b + c + D) :
        ppl_console_print( "Hellohi: %d\n", c);

    for the_iterator in 0 ..= 7 do
    {
        ppl_console_print("%d\n", the_iterator);

        if the_iterator then break;
    }

}

d :: 0