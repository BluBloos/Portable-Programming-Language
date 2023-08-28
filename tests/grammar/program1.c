A :: struct {
    a:int;
    b:int;
    c:char;
    aa:float;
}

// global variable!! :)
b :: int = 7; c :: int = 8;
d :: int = 9 // the semicolon is optional for compile-time vars.

main :: fn (argc:int, largeLad : A) ->int {

    if 5 > argc; printf("Mans is large");

    // really need to implement the j++ thing...
    while  j := 0; j < argc; j++; {
        *(^u8)@largeLad = j + 1; // this should be fun.
    }

    // simple scope
    {

        a:int = 4;
    }

}