// consider a new operator, "expand".
// `...`

// we allow this to be the only? operator that can take no
// params at all.
(...) == -inf... inf;  // true

// it can also be an unary operator, but accept on either side.
(0 ...) == 0 ... inf;  // true
(... 0) == -inf... 0;  // true

// we can see that with this operator, there is a new type called range.
range<int> A = 0 ... 10;          // ranges are templated.
range<char8_t> B = '👀' ... '👁️';  // picks all the unicode points between.
// a range is a set with strictly +1/-1 between each element in the set.

// TODO: could we have a float range if just have some stepSize?
// TODO: could we allow for ranges that have simple arithmetic between each
// element (like +2, +3, -4, etc)?

A[0 ... 10] += 10;
// with the above, we now have a "slice" of an array.
// the above expression doesn't look too bad, honestly.
// it would basically be as if you had a loop where you did += 10 to each
// element. and then you unfolded the loop. so our compiler can do this however
// it pleases. and we could say that this doesn't necessarily hide anything
// (runtime cost). i.e. the compiler should be able to reason quite well between
// the above and the below, to see they are equivalent.
for (int i = 0; i < 10; i++) {
    A[i] += 10;
}

// slices are types too.
// it goes as [begin, end). to match other languages.
slice<^int> B = A[0 ... 10];  // slices are just two raw pointer.

// here we use a range and an initializer_list to initialize a static array.
[]int A = {
    0 ... 10};  // an array with 11 ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// what if I try to set a slice to a range?
// the `=` operator allows for this and has the following definition:
A[0 ... 10] = 0 ... 10;  // this is like a loop where you do A[i] = i.
// here's the loop:
for (int i = 0; i < 10; i++) {
    A[i] = i;
}

// what if a slice to set into a slice?
A[0 ... 10] =
    A[10 ... 20];  // this is like a loop where you do A[i] = A[i + 10].
// again, I'm not sure I see any problem here in terms of hidden cost.
// like, here's the loop:
for (int i = 0; i < 10; i++) {
    A[i] = A[i + 10];
}

// so, these slices and ranges are bascially here to allow us to write more
// expressive code.
