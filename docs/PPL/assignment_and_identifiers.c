// IDENTIFIER DEFINITION:
// identifiers are human readable names that resolve to a tangible space in RAM.
// the RAM could correspond to any data section in the executable, a loc in the
// stack, or somewhere in the heap.
//
// More formally, identifiers resolve to a value type of UID<T>.
// The identifier type is not a value type. It is a special class of type that
// can be resolved to UID<T>.

// ASSIGNMENT EXPRESSION SEMANTICS.

// Idenfitiers are first class citizens of the compiler runtime (a type) and can
// therefore be the operands of operators.

// for the discussion that follows, allow the statement "bin <binName>" to mean,
// "tangible location at the UID for the <binName> identifier".

// consider the following expression,
int A;
int B;
A = B;
// It must follow, based on how we understand assignment to work in other
// languages, that the assignment operator itself is well-defined for:
// LHS type = identifier
// RHS type = any value type that can cast to T in @identifer = UID<T>.

// note that the '@' operator is a unary operator that takes an identifier and
// resolves it.

// the '=' operator will take the expressions on the left and right side and try
// to cast them if needed. this is expected as operators are function calls, and
// function calls do this sort of forgiving behaviour where they try to cast
// operands during overload resolution. See operator_overloading.cpp

// The LHS top-level expression has the type of identifier. No casting is
// required. Since @A = UID<int>, the RHS must have a value type of int. The B
// expression at the top-level is an identifier type. As such, we must cast to
// int. This is done by performing a read access into bin B.

// Generally, casting an identifier to a value type is first done by resolving
// it, then doing a read access with the UID<T> address, to ultimately get T.

// The final result of the expression evaluates (expressions always evaluate to
// a value) to the identifier on the LHS. It also has the side effect
// (specifically for this case) of a write access to A with the result of the
// bin B read access. More generically, it has the side effect, "write into bin
// LHS with value of RHS".

// the following examples will hopefully clarify the above.

// consider the below.
0xFFF89 = 10;
// this is _almost_ okay. the compiler may be forgiving and cast 0xFFF89 to an
// identifier. however, it could only ever be resolved to UID<?>. the ambigious
// type stops this expression from ever being valid.

// now consider
^int C;
int  B;
C = B;  // this reads the value in B and assigns it to C.
// identifier = identifier
// '=' sees an identifier on LHS. no cast needed for C.
// since @C = UID<UID<int>>, the RHS should have a type of UID<int>.
// any implicit cast from identifier -> UID<int> is not permitted.
// these must always be explicit. thus, the only permittble cast is:
// first read from B to get int, then up cast to UID<int>.

// if we wanted to do maybe what actually makes sense, we can do this:
C = @B;

// but, what if we do the following???
0xFFF89 + A = 10;  // invalid.
// the '+' operator casts the A identifier -> T (int) with a read access.
// then 0xFFF89 is cast as int for the '+'.
// no other '+' overload is allowable, so this is the only case.
// this is the case as again, we cannot cast identifier -> UID<int> implicitly.
// the result of the '+' op is an int type. this results in the total expression
// being invalid as the int cannot be cast back up to an identifier. the type
// information is not here for the mem addr. we do not know what type we would
// be pointing to.

// What if we did?
0xFFF89 + (^int)A = 10;  // valid.
// the cast operator (which is a unary op) takes A -> UID<int>.
// @ is the only method for cast identifier -> UID<int>.
// therefore the cast op cannot directly make this cast.
// first, A is read to get int, then this is up cast to UID<int>.
// '+' operator then casts 0xFFF89 -> UID<int>, the add occurs and we get back
// UID<int>. finally, UID<int> is up cast -> identifier. this sort of implicit
// up cast is also OK :D
// we'll note that this (^int)A cast is crude. who knows if what the int A
// contains is a valid mem addr. much better to:
0xFFF89 + (^int)@A = 10;

// Now consider how to do the whole deref thing.
^int D;
int  E;
^D = E;
// the deref operator takes an identifier and outputs aother identifier.
// it pulls the UID<T> out of UID<UID<T>>.
// then E on the left hand side is down-cast from an identifier direct to
// the int type by performing a read, again skipping identifier -> UID<T>
// (resolve), because this can never be implicit.

// BRIEF SUMMARY.
// so to summarize, cast from identifier has ONLY two path.
// 1. resolve,
// 2. read,
// resolve is always explicit and with @, to get the UID<T> direct.
// the read op gets the value exp with value type T in @identifier = UID<T> by
// reading from bin identifier.
//
// if we want to up cast to an identifier, we can do this from an UID<T>,
// and we can up cast from an integer type to UID<T>.
// the cast up from an integer won't typically happen unless we do (^int)A,
// as an integer literal has no type information, and writing directly A is an
// identifier.

// what about?
10 + ^D = 10;  // ill-formed. that's OK. also ill-formed in C.
// ^D gives back an identifier.
// this is then down cast read to add with 10.
// the exp is ill-formed because we don't have the type information for the LHS.
// we do not know what type we are point to and therefore cannot up cast to
// identifier for LHS requirement.

// what about this (important because it is a common case)?
10 + D = 10;
// 10 + identifier = 10;
// have to read (no implicit @).
// 10 + UID<T> = 10;
// 10 is up cast in the addition.
// then UINT<T> is up cast to an identifier.
// all is Good!
