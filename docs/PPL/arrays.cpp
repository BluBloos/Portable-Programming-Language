// let's first consider a static array.
// and to be consistent with the pointer syntax, they ought to go like this:
[10]int a;

// and

let const [10]int a;  // let there be a const array of 10 integers called a.
// again, the let is implicit.
const [10]int a;  // same as above.

// now, this is a thing that we love to see happen and it's called decay to
// pointer.
^int B;
B = a;  // and now B points to the first element of a.

// and note that our compiler should not allow the above, actually, because A is
// const. and through this pointer to not const, that's a mismatch.

// the value inside the [] is a constexpr.
[10 + 20]int C;

// since we have declared that what is inside [] is in fact an expression, what
// type is it?
constexpr size_t D = 100;
[D]int          E;  // this is a static array of 100 integers.

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

// --------- DYNAMIC ARRAYS, RANGES ------------

// In C++ there is std::vector<>.
// In C there is no such dynamic array idea. But we can do it ourselves.

// In enums.cpp we argued that to allow iteration over a disjoint enum would
// imply a hidden cost.

// If we wanted to offer a native dynamic array type, this too would imply a
// hidden cost. Because there isn't a just 1 way to do dynamic arrays.

// you could have different allocation strategies, different data structures,
// etc.

// if we want dynamic arrays and no hidden cost, we ought to just be clear what
// is happening, and just pick one particular technique. never swap the
// technique.

// So firstly, there must be some concrete language primitives that deal with
// alloc/delete memory. realloc is just a sequence of alloc and delete, so use
// can impl this if needed.
^int F = new [10]int;
delete F;
// these are of course things that the standard library must enable. Like `new`
// maybe on Win32 maps to VirtualAlloc, for example.

// and the dynamic array syntax is as follows:
[$]int A;  // where `[$]` is its own symbol thing used for dynamic array
            // declaration.

// what if I want to give the array a begin size?
[$]int A(10);  // here we use a ctor to give the array a begin size.
// the size it takes in the ctor is a size_t.


// the method that dynamic arrays work by is a simple one. they are just a
// pointer to a block of memory, and a size_t that tells you how many elements
// are in the array. an additional size_t could be used to tell you how many
// elements the array can hold before it needs to be resized.
//
// resize is realloc, so just use the new and delete operators.
//
// when resized, the array size is simply doubled.


// --------- DYNAMIC ARRAYS------------

// Consider this useful template func.
template <size_t Size>
size_t foo([Size] int &a) {
    return Size;
}

// here, we require that the param A is a reference type. If not, when pass by
// value there would be a pointer decay and we would lose the size information.
// so, these sorts of templates are reasons for references to exist (among other
// reasons).

// While maybe in the past I wasn't particularly a fan that there are both raw
// pointers and references. It does make sense. They are not the same concept.

// References sort of change how things fundamentally work.
// Like a pass by ref versus a pass by value is a fundamental difference.

// And you can have a reference to a pointer, too.