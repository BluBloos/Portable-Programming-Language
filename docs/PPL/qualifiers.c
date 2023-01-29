// -------------- HOW QUALIFIERS WORK ----------------

// a qualifier modifies strictly what is to the right of it.
// and it only modifies just one item.
// a qualifier cannot modify a qualifer, so it searches until it can find the
// first non qualifier. this allows for qualifier sequences.

// please see pointers.c to understand how this relates to pointers.

// ex)
const int A; 
// const modifies direct to its right.
// this becomes, "declare a constant integer called A".
// const sorta folds into the value type.

// qualifiers are unordered and mutually exlusive.
const static int a;

// interesting example.
int const A; 
// here, we mark the identifier itself as constant.
// recall from assignment_and_identifiers.c that @identifier = UID<T>.
// thus, a const identifier must mean that the UID is constant.
// for a typical variable, this is already implicit.
// but for a reference (if we choose to support them),
// this idea actually has some merit.


// -------------- STATIC --------------
// 1. not visible to other compilation units.
// 2. does not live on stack and has a const address in memory at runtime.
// this is kind of to say, "the address is static".

// -------------- CONST --------------
// this is SSA. It must be assigned a value and cannot be changed after.

// -------------- CONSTEXPR --------------
// this is a value that is known at compile time.

// -------------- INLINE --------------
// 1. when applied to func, hint that we should inline calls to this func.
// TODO: what does inline mean in other contexts?

// TODO: I understand that the exact meaning of qualifiers in C++ has
// changed across the years. I am not sure what the current meanings are
// and should do some reasearch.