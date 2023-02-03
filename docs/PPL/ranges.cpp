// consider a type called range.
range<int> A(0, 10);
range<char8_t> B('👀', '👁️');  // picks all the unicode points between.

// a range is a set with strictly +1/-1 between each element in the set.
// it should be noted that unlike an array, ranges do not store the entire set,
// they represent. they are just two values, begin and end.

// TODO: currently, we use `..` also for range.
range<int> A = 0 .. 10;
// but this is very similar to `...` which I hope to use for fold expressions and the sort.

// TODO: could we have a float range if just have some stepSize?
// TODO: could we allow for ranges that have simple arithmetic between each
// element (like +2, +3, -4, etc)?

// TODO:
// Suppose that I wanted -inf and inf, what type would I need?

// TODO: should we allow the below to be valid? to mean add 10 to each element?
A[0 .. 10] += 10;

// alternatives:

// this may look like Python, but it's not :D
A[0 .. 10] = [ A[i] + 10 for size_t i in range(10) ];
// this above kinda sucks, but it works.

// here's a template thing.
void foo()
{
    []int A = [0,1,2,3,4,5,6,7,8,9];
    range<int> myRange = 0 .. 10;

    // how to use fold-expressions but at runtime.
    template<int... IdxPack = myRange>
    (A[IdxPack] += 10) , ...;
}

// of course, we could always just do a for-loop:
for (int i = 0; i < 10; i++) {
    A[i] += 10;
}

// slices are types too.
// it goes as [begin, end). to match other languages.
slice<^int> B = A[0 .. 10];  // slices are just two raw pointer.

// here we use a range as an element of an arrary literal to initialize a static array.
[]int A = [ 0 .. 10 ];

// we could even use many ranges in our array literal.
[]int A = [0 .. 10, 20 ..30, 40 ..50];

// or a mixed form:
[]int A = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20 .. 30, 40 .. 50];

// what if I try to set a slice to a range?
// the `=` operator allows for this and has the following definition:
A[0 .. 10] = 0 .. 10;  // this is like a loop where you do A[i] = i.
// here's the loop:
for (int i = 0; i < 10; i++) {
    A[i] = i;
}

// what if a slice to set into a slice?
A[0 .. 10] =
    A[10 .. 20];  // this is like a loop where you do A[i] = A[i + 10].
// again, I'm not sure I see any problem here in terms of hidden cost.
// like, here's the loop:
for (int i = 0; i < 10; i++) {
    A[i] = A[i + 10];
}

// so, these slices and ranges are bascially here to allow us to write more
// expressive code.
