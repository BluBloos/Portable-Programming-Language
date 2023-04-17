// we use `!` for macros. Rust inspired.
// macros are strongly typed.
// they are not doing the C thing where it is a preprocessor thing.
// at least one of the purposes of such a macro is to force inline code instead
// of a weak `inline` keyword suggestion.

a! : (a:int, b:int) -> int {
    return a + b;
}

a(10, 20);

b! : (a : id, b : id) -> id {
    // `##` is normally used as the stringify operator in the unary case.
    // but when used in a binary context, it is the concatenation operator.
    // this is a macro that therefore concatenates the two identifiers.
    return a##b;
    // no whitespace for `##` is allowed as this is not in the grammar for identifiers.
}

c! : id = USELESS_MACRO;

// theres's this interesting idea where {} is actually
// a function that takes in nothing and returns nothing.
d! : {
    2+3;
    foo();
    bar();
}


