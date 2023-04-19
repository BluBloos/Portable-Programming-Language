// for some good discussion on operators, specifically the assignment one,
// please see assignment_and_identifiers.c

// also see operator_overloading.cpp

main : ()->int {  // prefer lawful good?

    a : int;
    b : int;

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

    // NOTE: While we might want ^ for raise to the power of, this is a little
    // high-level. stick to the basics, and let the user define their own pow().
    // or std lib.

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
    ^a;  // pointer dereference.
    // the dereference op is well-defined in assignment_and_identifiers.c

    @a;  // address of.

    ##1234;  // stringize operator. well defined in enums.cpp

    a ? b : c;  // ternary.

    // I invision member access to just be an offset from @identifier = UID<T>,
    // but where T needs to have the member as verified by SEMA.
    a.b;  // member access.
    // NOTE: member access works for what in C++ was :: and ->.
    // i.e. with namespace access it is also `.` and with member access through
    // pointer it is also `.`.

    a[b];  // subscript.
    a();   // function call.
    a++;   // post-increment.
    a--;   // post-decrement.
    ++a;   // pre-increment.
    --a;   // pre-decrement.
    a, b;  // comma.

    // C operator precedence
    // https://en.cppreference.com/w/c/language/operator_precedence, for
    // reference

    // TODO: think about pointer to member.

    return 0;
}