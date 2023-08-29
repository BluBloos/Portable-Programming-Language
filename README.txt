Portable Programming Language
===

Built with <3, this package contains the official compiler toolchain for the
Portable Programming Language.

PPL is a systems programming language with the goal of allowing the developer
to produce software, hassle-free, for any target platform.

Here is a small sample of PPL,

===
pal  :: #import "ppl.pal";
plib :: #import "ppl.types";

main :: fn () -> int {
    myIntegers := plib.Array<u32> {};

    for i in 0 ..< 10 {
        myIntegers.push(i);
    }

    pal.print("\nThere are % many integers\n", myIntegers.size);

    return 0;
}
===

This language takes inspiration from many other programming languages. To list
them all would be much too exhaustive. Notable mentions would be C/C++, Jai,
and Odin.

Foreword
===

This project is in HEAVY DEVELOPMENT.

The language is undergoing active design. The design currently leads while the
compiler support lags behind.

# Steps for Using

Simply execute the `ppl` binary. Therein contains an interactive program to
guide the user through performing any action that the PPL compiler toolchain
currently supports.
