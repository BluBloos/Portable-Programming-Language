// .extern p_decl void ppl_console_print(int64, []int64)

b :: int = 7; c :: int = 8;

main :: fn () -> int
{
    D : bool = 9u;  // testing that booleans can only store 0 or 1.

    C : char = 150; // testing that char is signed.

    F : short = (u8)256 + 32766;  // testing truncation + upcast in expression.

    // NOTE: the behavior of this expression varies w.r.t. C. In C, the 167 + 167
    // part will not be restricted to u8 bitwidth. So in C we get a result of 33100.
    Z : int = ( (u8)167 + (u8)167 ) + 32766;

    ZZ : char = ( (u8)167 + (u8)167 ) + 32766;

    ZW : int = C + D + Z;

    //       18446744073709551615
    e : int = 9223372036854775807; // testing print limit.

    D ? ppl_console_print( "Hello: %d\n", b + c + D ) :
        ppl_console_print( "Hello: %d\n", c );

    // NOTE: the result of this expression varies w.r.t. C. the final addition result
    // is cast to short, then to int.
    W : int = (u8)Z + (short)ZW + (s8)e;

    B : int = 32800;
    A : short = B;

    CC : char = ( (u8)167 + (u8)167 );

    // NOTE: the variadic function stuff cannot do type checking
    // because we do not know upfront what the types of the variadic args are.
    // so we have to cast these to int64 for printing as we need to match with %d.

    ppl_console_print("The value of D is: %d\n", (int)D); // expect 1.
    ppl_console_print("The value of C is: %d\n", (int)C); // expect -106.
    ppl_console_print("The value of F is: %d\n", (int)F); // expect 32766.
    ppl_console_print("The value of Z is: %d\n", Z); // expect 32844.
    ppl_console_print("The value of ZZ is: %d\n", (int)ZZ); // expect 76.
    ppl_console_print("The value of ZW is: %d\n", ZW); // expect 32739.
    ppl_console_print("The value of W is: %d\n", W); // expect -32722.
    ppl_console_print("The value of A is: %d\n", A); // expect -32736.
    ppl_console_print("The value of CC is: %d\n", CC); // expect 78.
    ppl_console_print("The value of e is: %d\n", e); // expect 9223372036854775807.

    for 0 ..= 7 do
    {
        ppl_console_print("it's loop time dude!\n");

        if d then
        {
            ppl_console_print("d is nonzero\n");
        }
        else
        {
            ppl_console_print("d is zero\n");
        }
    }

    return 10;
}

d :: 0
