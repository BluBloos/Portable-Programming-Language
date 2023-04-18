// this is an array. the value in [] must be known at compile-time.
a : [10+20]int;

// decay to pointer works no problem. of course, we lose all information about the array, but
// that's simply the "cost" of decay to pointer. 
pA : ^int = a;

// this is a constant array. none of the values inside the array can be changed.
b : const [2]float;

// the compiler should flag this as an error. this cannot work since through the pointer
// we would be able to change something that is const.
pB : ^float  = b;

// instead of having constexpr we can used our strongly typed macros.
c! : size_t = 100;

d : [c]int;

// the definition of size_t is:
// size_t can store the maximum size of a theoretically possible object of any
// type (including array).
//
// if we had an array of uint8_t, it could maximally have MAX_SIZE_T elements,
// because MAX_SIZE_T is the maximum size of any object. the maximal index of
// such an array is MAX_SIZE_T - 1. for any array of a type with more than 8
// bits, it can maximally have some number of elements less than MAX_SIZE_T. so
// we can see quite clearly that size_t is a suitable type for the index of
// array objects.

// TODO: I think the reason I was using [] was so that the syntax is specific for array
// initialization over something like {} which is this whole other idea.
F : [10]int = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

// this is possible.
foo : <Size:size_t> ( a : [Size]int ) -> size_t {
    return Size;
}

// but,

// we could also just do:
foo : (a : []int) -> size_t {
    return countof(a);
}

// countof() is a compiler intrinsic.

// dynamic memory allocation is an operating system specific function.
// therefore, there is no `new` keyword.

// the best that we can do is have the `malloc` function as part of the standard lib.
F : ^int  = malloc( sizeof(int)*10 );
free(F);


// therefore, something like dynamic arrays _cannot_ be a core part of the language.
// it is a part of the standard lib of the lang.


// consider std::move, an impl of _move semantics_.
// in the words of Bjarne Stroustrup:
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2027.html#Move_Semantics
// > This move() gives its target the value of its argument, but is not obliged to
// > preserve the value of its source. So, for a vector, move() could reasonably
// > be expected to leave its argument as a zero-capacity vector to avoid having
// > to copy all the elements. In other words, move is a potentially destructive
// > read.

// 2. emplace_back.
// most of the time the whole move() idea is done by the compiler.
// it sees that you create a temporary and it automatically
// creates it inside the array rather than creates it, then copies.
// but if we already have another var, we can move() into the array.
// now the var B may never be used again.
//
// the semantic of the move() built-in (compiler intrinsic) is like:
// "take the thing and create is as a temporary here, and destroy all other references to it."
A := [1u, 2];
B := 2u;
A[0] = move(B);