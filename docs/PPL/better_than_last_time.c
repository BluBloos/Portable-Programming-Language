
// This isn't my first rodeo! My first compiler project is called Plasma
// Compiler. https://github.com/BluBloos/Plasma-Compiler

// The aim of this page is to track the development of the Portable
// Programming Language compiler toolchain relative to the progress that was
// reached in my previous compiler project, Plasma Compiler.
//
// The question is simply: "Did I do better than last time?"

// Below is a listing of factors to evaluate both projects on. A partial
// implementation e.g. of just generating the AST won't be evaluated on these
// factors.

// FACTORS
// =======

// 1. Language Expressiveness
// 2. Speed of Compilation
// 3. Speed of Output Binaries
// 4. Ease of use
// 5. Platforms that the compiler can run on
// 6. Platforms that the compiler can compile for
// 7. Amount of dependencies
// 8. Error messages when the user program is incorrect
// 9. Source language it was written in
// 10. The standard library features

// PPL VS. PLASMA COMPILER
// ==============================================
// ==============================================

// LANGUAGE EXPRESSIVENESS
// ==============================================
// Status: PPL is NOT winning.

// PLASMA
// ======
/*
1. Single line comments (do not support multi-line).
2. Function definition; they don't need to be ordered.
3. Variable declaration and assignment (do not support global vars).
4. Variable Scoping; like, shadowing is all good.
5. If, else if, and else
6. For loops
7. While loops
8. Break / Continue; unclear how it works if there is nested for loop. does break exit
   all loops or just the immediate loop?
9. Recursion
10. Precedence in expressions
11. Ternary operator
12. Supported binary operations:   +   -   *   /   ==   !=   >   <   >=   <=   ||   &&
13. Supported unary operations:   -   !
14. only supported type is int. we treat int as 32 bits. it's unclear if plasma
  handles signedness correctly.
*/

// PPL
// ===
/*
1. Single and multi-line comments.
2. TODO: function definition.
2. Global and function-local variable declaration.
    TODO: assignment to runtime variables.
4. TODO: variable scoping.
5. If, else if, and else
6. compile-time, Span based For Loops.
7. while loops take the form of legacy for loop syntax.
8. break and contiinue statements.
9. TODO: recursion
10. Precedence in expressions
11. Ternary conditional operator
12. Supported binary operations:   +   -
13. Supported unary operations:   type cast 
14. support for all integer types of 8, 16, 32, and 64 bits. support for signed
  and unsigned.
15. TODO: add support for switch statement using jump tables.
*/


// TESTING SPEED OF COMPILATION
// ============================
// Status: Unknown.

// TODO: https://github.com/RealNeGate/Cuik/blob/master/tests/bigboy.c
// write .c -> .PPL tool.
// convert the big file to minifed version so it can run in Plasma.

// PLASMA
// ======
// ?

// PPL
// ===
// ?


// TESTING SPEED OF OUTPUT BINARIES
// ================================
// Status: Unknown.

// TODO: https://benchmarksgame-team.pages.debian.net/benchmarksgame/fastest/rust-gpp.html

// PLASMA
// ======
// ?

// PPL
// ===
// ?


// EASE OF USE
// ================================================
// Status: Both easy to use.

// PLASMA
// ======
// a single cmdline command to compile a file.

// PPL
// ===
// interactive command line prompt.
// `cl` to compile, `r` to run the previously compiled file.


// COMPILER HOST PLATFORMS
// ================================================
// Status: PPL winning.

// PLASMA
// ======
// Windows.

// PPL
// ===
// macOS, Windows.


// COMPILER TARGET PLATFORMS
// ==================================================
// Status: PPL winning.

// PLASMA
// ======
// Windows.

// PPL
// ===
// macOS, Windows.


// DEPENDENCIES
// =================================
// Status: PPL winning.

// PLASMA
// ======
// Python, Visual Studio 20XX, NASM.

// PPL
// ===
// Visual Studio 20XX, NASM.


// COMPILATION ERRORS
// =================================
// Status: PPL has the potential to win but there is still work to do here.

// PLASMA
// ======
// example errors:
/*
[ERROR]: line 1 expected (
[ERROR]: symbol float h is not defined
*/

// PPL
// ===
// example errors:
/*
[ERROR]:
tests/preparse/neg/string_literal.c:2,13: Unclosed string literal. Began at 2,13.

	// missing `"` below.
	{'3'; Hello!"; 5;}
	
*/
// NOTE: the above ^ is a printing of the source where the error happened.
// It cannot be seen here, but in actual usage the erroneous part is colored
// red.


// SOURCE LANGUAGE
// ==============================
// Status: PPL winning.

// PLASMA
// ======
// Written in Python.

// PPL
// ===
// Written in C++.


// STANDARD LIBRARY
// ===============================
// Status: PPL winning.

// PLASMA
// ======
// No standard library.

// PPL
// ===
//
// .extern p_decl void ppl_console_print(int64, []int64)
// .extern p_decl void ppl_exit(int32)
//
// ppl_console_print is a variadic function. It's like printf where the first
// argument is the format string, and the remaining arguments are based on
// the format string. Only `%d` is supported, which prints 64 bit signed integers.