// when I consider how the language parses / interprets things.

// it makes sense that overloading operators is like "define a global function".

// within the class doesn't make sense.

// it's not the class itself that is doing the operator.

// its the compiler runtime that looks at LHS and RHS and the operator, then makes sense out of that expression.

// so unless there is a deep need, operator overloading should occur as global funcs.

// ex)

struct A
{
    float x;
}

struct B
{
    double w;
}

double operator+(A a, B b)
{
    return a.x + b.w;
}