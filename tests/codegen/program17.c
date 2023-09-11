b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D := 9u;
    D ? ppl_console_print( "Hello: %d\n", b + c + D) :
        ppl_console_print( "Hellohi: %d\n", c);

    for the_iterator in 0 ..= 7 do
    {
        ppl_console_print("in the loop\n");
        if the_iterator then continue;
        ppl_console_print("the_iterator: %d\n", the_iterator);
    }

}

d :: 0