// support for bitfields.
struct MyStruct {
    int a : 3;
    int b : 5;
    int c : 4;
};

// bitfields just restrict the range of possible values for our var. it is
// functionally less bits. this means that arithmetic operations on bitfields
// are not allowed, unless they have the same width.

// bitfields are of course allowed outside of sructs. as normal vars.
// but they cannot have a width larger than the type they are declared as.
short a : 17; /* Illegal! */
int   y : 33; /* Illegal! */

// thus, the bool type is actually an alias for uint8_t : 1!
bool myBitfield;

// ------ MEMORY LAYOUT ------
// how the compiler lays structs out in memory: for speed.
// this means things get aligned by default.
// even if we use bitfields.

// how can we make things pack?
// in C it varies by compiler.
// we'll just have to pick a syntax and stick with it.
// please see attributes.cpp where we go over this syntax.