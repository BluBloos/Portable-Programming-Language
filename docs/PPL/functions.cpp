// functions are first class citizens

// we can do the typical stuff like:
// pass them as arguments to other functions.
// return them from functions.

// but consider this:

foo : () -> ((x:int) -> int) { // no equal sign required for {}.

    // TODO: maybe think about the whole usage of static and how there are different
    // meanings such as static storage duration and static linkage.
    a : static = 1u;

    // functions can exist anywhere.
    // every function ever has its code static in the .exe.
    // the function type is simply a pointer to the function code.
    bar : (y:int) -> int {
        return a + y;
    }

    // the variable `bar` is scoped to this function and therefore
    // normally the static code cannot be called from anywhere else.

    // however, since the pointer to the static code is returned
    // through this function, it may.

    // this brings up the discussion on the capture of `a` here.
    // how does that work?

    // firstly it depends on if `a` has static storage duration or not.
    // capture by reference would therefore be no issue.

    // however if the lifetime of the thing is temporary, we have to be careful
    // in the capture by reference case.
    
    // the rule is that the lifetime of the temporary must be less than or equal
    // to the lifetime of the function pointer. otherwise a compiler error will occur.
    // that sounds hard, but maybe it is possible. we'll see. for now lets assume it it :)

    return bar;

} // no-semicolon required for {} blocks, ever.

// any function declared at the global scope is implicitly static.
foo2 : {
    
    a : int=1; // undetermined at runtime. not initialized. this can be more efficient and maybe save
    // a single instruction afaik?

    // capture by value (this is also a template)
    // when we capture by value this is a value type template parameter with the name of `a` and value of a.
    // which therefore means this template is deduced at declaration time, not call time.
    bar : <a> () -> int {

        // TODO: George said the <> may be confusing for capture by value. 
        return a;
    }

    tFunc : <T:uint32_t, a> (b:T)->T {
        return a + b * 2;
    }

    b := tFunc<2>(4);

}

myFunc := foo();