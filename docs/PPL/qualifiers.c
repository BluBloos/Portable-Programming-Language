// -------------- HOW QUALIFIERS WORK ----------------

// a qualifier modifies strictly what is to the right of it.
// and it only modifies just one item.
// a qualifier cannot modify a qualifier, so it searches until it can find the
// first non qualifier. this allows for qualifier sequences.

// please see pointers.c to understand how this relates to pointers.

// ex)
A : const int; // "let A be a constant integer".

// const sorta folds into the value type.

// qualifiers are unordered and mutually exclusive.
a : const static int;

// -------------- STATIC --------------
// 1. not visible to other compilation units.
// 2. static storage duration which implies const address in memory at runtime.

// -------------- CONST --------------
// this is SSA. It must be assigned a value and cannot be changed after.
