A:struct {
    a:int;
    b:int;
     c:char;
     aa:float;
};

b:int = 7; // global variable!! :)

 main: (argc:int, largeLad:A) ->int {

    // Lmao note that the . operator simply is not implemented. So sad!
    if (5 > argc) {
        printf("Mans is large");
    }

    // really need to implement the j++ thing...
    for (j:int; j < argc; j = j + 1) {
        argc = j + 1; // this should be fun.
    }

    {
        // simple scope
        a:int = 4;
    }

}