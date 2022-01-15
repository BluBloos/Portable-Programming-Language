// C operator precedence, for reference
// # https://en.cppreference.com/w/c/language/operator_precedence

// NOTE(Noah): Apparently everything good about language design has already been done...
// So what if we made PPL flexible? -> metaprogramming?



// NOTE(Noah): Currently considering range based for-loops.
    // because there are common patterns where I want to change the index as it iterates.

// NOTE(Noah): Currently considering implicit dereference.
    // because if you have this, then you might not know that you are dealing with a pointer.
    // this is not explicit, and I generally dislike a lack of explicitness.
    // NOTE(Noah): But what if there was some other feedback mechanism?
        // Like it syntax highlighted your pointer variable?    

/* TODO

- Read the Rust langauge specification to learn how they make memory safe.

- Package management
- Port C baclend to ANSIC
- Add LLVM IR as backend

- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- >> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.
- double variable initialiazation
    Ex) int last_x, last_y;
- add single character printing of unicode code points.
- "stringification" of enums.
- Can define structs/functions anywhere. Like: inside another function if wanted.
- vector and matrix math ops are first class citizens of the language.
- lambda functions.
- map/dict type (hash table is first class)
- 0b01010101 (binary literals)
- 0xFF (Hexadecimal literals)
- Complex numbers, 2i + 3
- 1.0e9 (scientific notation)
- 1_000_000_000 (underscores for better readability)
- 64 bit floating point numbers
- struct literal
v = Vector3{1, 4, 9}
- make Quaternions a default type.

Check out the things below,
- Consider branching and Phi nodes for compiler optimization.
- Check out what vectorizin is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
*/