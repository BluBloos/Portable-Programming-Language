A :: struct {
    a:int;
    b:int;
    c:char;
    aa:float;
}

b :: int = 7 // global variable!! :)

main :: (argc:int, largeLad : A) ->int {

    if 5 > argc {
        printf("Mans is large");
    }

    // really need to implement the j++ thing...
    while  j := 0 ; j < argc; j++ {
        ^(^u8)@largeLad = j + 1; // this should be fun.
    }

        // simple scope
    {

        a:int = 4;
    }

}