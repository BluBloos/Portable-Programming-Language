// I argue that operator overloading should occur as a global function.

// it's not the class itself that is "doing" the operator.
// its the compiler runtime that looks at LHS, RHS and the operator, then makes
// sense out of that expression.

// so unless there is a deep need, operator overloading should occur as global
// funcs.

// ex)

struct A {
    float x;
}

struct B {
    double w;
}

double
operator+(A a, B b) {
    return a.x + b.w;
}