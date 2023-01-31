// functions are first class citizens

// we can do the typical stuff like:
// pass them as arguments to other functions.
// return them from functions.

// but consider this:
int foo(int x) {
    int bar(int y) { return 1 + y; }
    // bar is what we call a scoped function declaration.
    // it's not a lambda, as that would be an entirely different beast
    // altogether. but `bar`, as a scoped function decl, is effectively a normal
    // function but scoped. it also implies a var declaration of `bar`.
    // the code in the executable for the func is static. the `bar` variable
    // itself is not necessarily static, but it is constexpr.
    //
    // as a non-lambda, it cannot do any capturing at all. this ensures the
    // constexpr property.

    // anyways, we can also do this (assign a function, not a lambda, to a
    // variable).
    // the function body is static. however, unlike the syntax above, `bar2` is
    // not constexpr. `bar2` can be reassigned to another function body if
    // desired.
    func<int, int> bar2 = int(int y) { return y + 1; };

    // also, for some notes on some of the syntax above...

    // we let there be a func type.
    // you template it with the return type and the argument types.
    // generally it is func<T_Return, T_Args...>

    // so in fact, every function declaration is implicitly a var declaration
    // like the examples above. global functions are just global constexpr vars.
}