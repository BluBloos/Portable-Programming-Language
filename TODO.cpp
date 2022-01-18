// C operator precedence, for reference
// # https://en.cppreference.com/w/c/language/operator_precedence

// NOTE(Noah): Apparently everything good about language design has already been done...
// So what if we made PPL flexible? -> metaprogramming?

// NOTE(Noah): Currently considering range based for-loops.
    // because there are common patterns where I want to change the index as it iterates
    // and I feel that this is odd when using a range based for-loop.

// NOTE(Noah): Currently considering implicit dereference.
    // because if you have this, then you might not know that you are dealing with a pointer.
    // this is not explicit, and I generally dislike a lack of explicitness.
    // NOTE(Noah): But what if there was some other feedback mechanism?
        // Like it syntax highlighted your pointer variable?    

/* TODO

NOTE(Noah): There are complexities with the package system that will only become apparent once we start to implement it.
Thus, we stop with what we have defined here and simply get on with our day. 
Desired Package System design:
Gonna use the keyword, "import".
- You have a file, and at the top you specify what your code is
dependent on. That's it. Every package is namespaced.
    -> It might be a header file that defines some routines. Maybe you will be linking against object files
    in the build step?
    -> A single header file library ( aka another .ppl file or a .h/.c/.cpp file ).
        -> This is to get at the single-file compilation model
        -> These are automatically include guarded
- Implications of this package system
    - Can import the main file into sub-components to leverage global variables that might be stored in the 
    main namespace.
- Each namespace defines orderless, callable methods and ordless variables with storage.
    - global variables are initialized in an orderless manner and deinit in orderless manner.
    - but this happens at the start of the program and then they all are destructed at the end of the program.

First Class Priority Tasks:

    - Port Compiler codebase to C/C++.
        - The lexer understands that files may be in utf8.
            - Leads to any utf8 char literals being parsed as symbols.
        - Add a preparser that does nothing but implementing a package system (port preparser.py).
        - Port grammer.py and tree.py and add appropriate tests in tests.cpp
        - Port syntax.py for AST tree generation and add appropriate tests in tests.cpp
        - Port codegen.py and linker.py to complete the port to C
            - While we are doing this, ensure the C backend is actually a C backend, not a C++ backend.
            - But this begs the question, which version of C? C89? C99? Modern C?
            - Here is how I am going to make my decision.
                - This is a MODERN language, and as such, we are going to use MODERN C.
                - Simple as that.
                - Because we can expect compilers to support this.
            - AND because the C backend is going to become irrelevant (likely) as soon as we implement the LLVM
            IR backend.

    - Is the language in a usable state, and are we satisfied with the language?
        - Look at the C89 spec and your own projects to answer this question.
        - C89 spec: http://port70.net/~nsz/c/c89/c89-draft.html#2.

    - Things to add to language to get into "usable state" / to feel "satisfied".
        - Verify that names of functions are legal (cannot begin with $ for example)
        - Can define structs/functions anywhere. Like: inside another function if wanted.
        - 0b01010101 (binary literals)
        - 0xFF (Hexadecimal literals)
        - 1.0e9 (scientific notation)
        - struct literal
        v = Vector3{1, 4, 9}
        - Add all desired variable types and sizes Ex) 
            - dynamic arrays
            - maps
            - string (a utf8 string).
            - char (a unicode coidepoint). 
            - uint64 (unsigned 64 bit integer).
            - float (32 bit floating point).
            - double (64 bit floating point).
            - vector
            - matrix
        - Ensure all operators are in the language
            - linear algebra ops with vectors and matrices.
            - add the len() operator for getting the length of
                - strings
                - dynamic arrays
                - static array
            - Add bitshift operators. Ex) >> and <<.
            - Ensure all postfix and prefix are there Ex) ++X or X++
            - Add -> for dereffing pointers. OR decide on implicit dereferencing.
        - Ensure standard library has what it needs
            - Single character printing of unicode code points.

    - Add LLVM IR as backend
        - Compile as an object file, then link with the standard library.

Other Tasks (Can be done anytime):
- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- >> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.
- double variable initialiazation
    Ex) int last_x, last_y;
- "stringification" of enums.
- lambda functions.
- Complex numbers, 2i + 3
- 1_000_000_000 (underscores for better readability)
- make Quaternions a default type.

Check out the things below,
- Consider branching and Phi nodes for compiler optimization.
- Check out what vectorizin is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
*/