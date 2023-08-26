A :: struct
{
    b : float;
    c : float;
}

// these are compile-time constants!! :)
b :: int = 7; c :: int = 8;

main :: fn () -> s32
{
    myStruct : A = {};
    myStruct.b = 3 + b;
    myStruct.c = 6 * c / d;

    if ComputeDidWin(myStruct.c, myStruct.b)
    then
        printf("You won! The winning numbers were (%f, %f)\n", myStruct.c, myStruct.b);
    else
        printf("You lost, and that sucks.\n");
}

ComputeDidWin :: fn (a : float, b : float) -> bool
{
    if a * b < 10.f then
       return true;
    return false;
}

d :: int = 9 // the semicolon is optional for compile-time vars.
