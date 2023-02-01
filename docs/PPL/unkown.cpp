// Consider a new sort of symbol, '?' = unknown.
// I can think of at least one use-case for this.
int A = ?; // explicit "we don't know" value.
// so you should never be allowed to:
int A; // because we prefer explicit ?.
// of course, this could also be a compiler switch to say, "allow implicit ?".

// it could also be used to explicitly NOT call any ctor at all for an object.
struct myType {
    int a;
};
myType A = ?; // just alloc on stack space for myType, do not call default ctor.

(?...?); // does this have any meaning?
// yeah, it does. it's a range where begin and end are noop.
// please see ranges.cpp