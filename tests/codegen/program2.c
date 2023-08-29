b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    return d ? print(
        "Hello, %d!\n", b) : 
               print(
        "Hello, %d!\n", c);
}

d :: 0
