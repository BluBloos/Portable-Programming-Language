// for some good discussion on operators, specifically the assignment one,
// please see assignment_and_identifiers.c

// also see operator_overloading.cpp

int main() {  // prefer lawful good?

    int a;
    int b;

    // binary operators.

    // clang-format off
    a + b;  // addition.
    a - b;  // subtraction.
    a * b;  // multiplication.
    a / b;  // division.
    a % b;  // modulus.
    a & b;  // bitwise and.
    a | b;  // bitwise or.
    a ^ b;  // bitwise xor.
    // clang-format on

    // NOTE: While we might want ^ for raise to the power of, this is a little high-level.
    // stick to the basics, and let the user define their own pow(). or std lib.

    a << b;  // left shift.
    a >> b;  // right shift.
    // clang-format off
    a && b;    // logical and.
    // clang-format on
    a || b;   // logical or.
    a == b;   // equality.
    a != b;   // inequality.
    a < b;    // less than.
    a > b;    // greater than.
    a <= b;   // less than or equal to.
    a >= b;   // greater than or equal to.
    a = b;    // assignment.
    a += b;   // addition assignment.
    a -= b;   // subtraction assignment.
    a *= b;   // multiplication assignment.
    a /= b;   // division assignment.
    a %= b;   // modulus assignment.
    a &= b;   // bitwise and assignment.
    a |= b;   // bitwise or assignment.
    a ^= b;   // bitwise xor assignment.
    a <<= b;  // left shift assignment.
    a >>= b;  // right shift assignment.

    // unary operators.
    -a;  // negate.
    !a;  // logical not.
    ~a;  // bitwise not.
    // clang-format off
    ^a;  // pointer dereference.
    // dereference:
    // means to "peek inside".
    // variable are just human readable names that point to some loc (some storage bin).
    // if we have a pointer variable, the bin stores a mem addr.
    // deref the pointer and we go through one level of indirection to get to another bin.
    // so if we have pointer A. Then (<A), this is now as an expression, a label for a bin.
    //
    // we could even do something as wild as <0x1234, which is a label for a bin.
    // clang-format on

    @a;  // address of.

    a ? b : c;  // ternary.

    // I invision member access to just be an offset from @identifier = UID<T>,
    // but where T needs to have the member as verified by SEMA.
    a.b;        // member access.

    // TODO: for now, this is member access through a pointer.
    a->b;

    a[b];  // subscript.
    a();   // function call.
    a++;   // post-increment.
    a--;   // post-decrement.
    ++a;   // pre-increment.
    --a;   // pre-decrement.
    a, b;  // comma.

    // TODO: think about pointer to member.

    return 0;
}