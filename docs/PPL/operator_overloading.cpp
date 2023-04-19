// I argue that operator overloading should occur as a global function.

// it's not the class itself that is "doing" the operator.
// its the compiler runtime that looks at LHS, RHS and the operator, then makes
// sense out of that expression.

// so unless there is a deep need, operator overloading should occur as global
// funcs.

// ex)

A : struct  {
    x : float;
}

B : struct {
    w : double;
}

operator+ : (a:A, b:B) -> double {
    return a.x + b.w;
}