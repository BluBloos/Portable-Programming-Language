// objects are basically structs with funcs.
// they are concrete types that take up memory.
// functions are stored somewhere in the .exe.
// there is no vtable. and thus there is no runtime polymorphism.

// this should be consider the creation of a variable called `MyBoy`
// with a structure type. we can use this var name later as a type
// if desired.
MyBoy : struct {

    // also note that if you remove the "struct" keyword the shits
    // would still compile. this just becomes a function now.

    m_a : int;
    m_b : int;
    Add : () -> int
    {
        return m_a + m_b;
    }
}; // `;` is optional. never required for `{}`.

// thus if I go:
MyBoy.m_a; // this works.
// regardless of if m_a is declared static or not.

thing := MyBoy();
thing.m_a; // this accesses a diff memory location to MyBoy.m_a; 

// but if,

boy2 : struct {
    m_a : static int;
}

thing2 := boy2();
thing2.m_a; // this accesses the same mem loc as boy2.m_a;

main : {

    // we can of course define vars with struct type anywhere.
    point : struct {
        int32_t x;
        int32_t y;
    };

    // nested structs work :D
    epic : struct {
        another_struct : struct {
            a:int;
            b:int;
        }
    }
}