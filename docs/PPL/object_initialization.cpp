// TODO:
// Suppose that I wanted -inf and inf, what type would I need?

// ------ INITIALIZATION ------

// semantic of   `{}` is to group a concrete set of data.
// so we can use `{}` to initialize variables - any type of variable.
// be it a function, struct, array, etc.

// this is called "data pack assignment", or something like that.
// the data unfolds logically into the linear list of members in the data structure.

B : struct {
    a : int;
    b : float;
}

/* last semi-colon can be omitted in a data pack. */
B = { 1; 1.f }

a := 10u;

// we can also use "designated initializers" for safer code.
// like if you change the struct order this still works in the future.
B = { .a = a; .b = 1.2f }

// the syntax above with nothing on the left side of the `a` is 
// used in data pack assignment contexts to re-slot the logical data pack
// member to another logical slot.

// if we use this same syntax when the data pack is a list of instructions, this
// is global namespace access.
{
    a := 100u;
    .a = 100; // sets the global-scope a to 100.
}

// not allowed. designated initializers to array slots must be in order
// WHEN USED WITH non-designated initializers.
A : [10]u32 = { .3 = 1; .0 = 2; 3; }; // gives compiler-error.

// gives an array of [ 2; 3; ?; 1; ?; ?; ?; ?; ?; ? ].
// so there's an explicit idea here. unless init, the memory is not set to anything.
A : [10]u32 = { .0 = 2; 3; .3 = 1; };

// ------ STATIC ARRAY INITIALIZATION ------