// HOW QUALIFIERS WORK:
// maybe say that qualifier modifes strictly what is to the right.
// it only modifies just one item.
// a qualifier cannot modify a qualifer, so it searches until it can find the first non qualifer. this allows for qualifier sequences.

// please see pointers.c to understand how this plays into pointers.

// qualifiers come before type.
// this is how ppl think.
const int a;

// qualifiers can be unordered.
// they are mutually exlusive, right?
const static int a;

// What does STATIC mean?
// 1. not visible to other compilation units.
// 2. does not live on stack and has a const address in memory at runtime.
// this is kind of to say, "the address is static".

// CONST
// this is SSA. It must be assigned a value and cannot be changed after. 

// CONSTEXPR
// this is a value that is known at compile time.

// INLINE
// 1. when applied to func, hint that we should inline calls to this func.
// TODO: what does inline mean in other contexts?