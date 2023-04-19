// Consider a new sort of symbol, '?' = unknown.
// I can think of at least one use-case for this.

A : int = ?; // explicit "we don't know" value.

// so you should never be allowed to:
A : int; // because we prefer explicit ?.

// of course, this could also be a compiler switch to say, "allow implicit ?".

// it could also be used to explicitly NOT call any ctor at all for an object.
myType : struct {
    a : int;
};

A: myType  = ?; // just alloc on stack space for myType, do not call default ctor.
