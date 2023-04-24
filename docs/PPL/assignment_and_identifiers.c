// IDENTIFIER DEFINITION:
//
// identifiers are human readable names that resolve to a tangible space in RAM.
// the RAM could correspond to any data section in the executable, a loc in the
// stack, or somewhere in the heap. the memory at this location is known to be of a
// particular type.
//
// Identifiers are first class citizens and thus have a type.
//
// The type of an identifier is `Id`.
//
// expressions of type `Id` can be resolved to their address with the unary `@` operator.

// lets play around,

// so all that we are doing is writing in some language that maps
// directly to machine code. just a set of machine instructions + data
// cached in the binary.

A : int; // so we define a runtime variable.
// since this is a global variable, it has static storage duration.
// thus this emits some empty storage space in the .exe to store this variable.

A = 3;   // and now we communicate a machine instruction that writes 3 to that storage space.

// so the idea of that empty storage space and tracking that the name `A` refers to it
// is an entirely compile-time idea.

// hence why for every variable declaration the compiler meta program creates an `Id` to track
// that variable.

// so e.g.

A_id :: Id;

// and the definition of Id is like so,
Id :: struct
{
    type : Type;
    address : ^void; // so the `@` operator is just using this.
    name : string;
}

// so let us say that the compilation metaprogram is looking again at this line:
A = 3;
// this object is an assignment expression AST node.
// the LHS is the string "A".
// the compiler takes this name and does a lookup with the current list of `Id`s to find
// the matching one. then that `Id` is returned as the LHS expression value. any `.` member
// access after that simply continues the same lookup process.

// and so yeah, the unary `@` operator works on the `Id` type and resolves where that sucker is.
#assert type_of(@A) = ^int;


// but let's consider a little more of an advanced example,

B : int;
A = B;

// the LHS type is expected to be `Id`.
// and the RHS type is expected to be `A_id.type`.

// the key idea here is casting. there are rules in the PPL language for
// casting expressions between types. implicit casting is allowed so long
// as there is not data loss.
// TODO: what are the rules for implicit casting?

// so the lookup of "A" gives us A_id and the lookup of "B" gives us B_id.
// the expression already has the type of `Id`. No casting required.

// However, the RHS expression also has type of `Id` but needs to be cast to a type of `A_id.type`.

// Here's the trick, a read access of the value contained at the .address of B_id is done
// as the cast operation.

// So the generic idea is in order to cast an `Id` to any other type, a read access is done.

// And boom! That's it. The final expression `A=B` results to the identifier on the LHS
// after writing into the bin at A_id.address what the value at the address of B_id.address was.

// let's consider some more fun stuff.

// consider the below:
0xFFF89 = 10;
// the thing on the left hand side is not an `Id`. the only way to get those
// is via the lookup thing.

// now consider:
C : ^int;
B : int;
C = B;  // this reads the value in B and assigns it to C.
// so we need to convert the `Id` on the RHS to a `^int`.
// again, the only mechanism is a read access via B_id.
// once we get the `int` value, a cast can be done to ^int.
// there is no data loss this is OK, but we should probably emit
// a warning.

// if we wanted to do maybe what actually makes sense, we can do this:
C = @B;

// but, what if we do the following???
// recall that A : int;
0xFFF89 + A = 10;  // invalid.
// simply put the LHS is not an `Id` type but an `int` type.
// therefore this is invalid.

// What if we did?
0xFFF89 + cast<^int>(A) = 10;  // valid.
// cast will want to take an `int` as input. there we read A,
// then cast that integer to be interpret as ^int.
// then do a little bit of pointer arithmetic.
// so the final result of the LHS expression is of type ^int.
//
// however, we cannot go much further here. ^int is not `Id`.
//
// to be correct we would have to do the below:
^(0xFFF89 + cast<^int>(A)) = 10;
{ // and also consider this
    D:^int;
    E:int;
    ^D = E;
}
// so the deref operator takes a pointer and gives back an identifier.
// so e.g. ^D first must read from D to get out the pointer type from `Id`.
// then the deref operator can do its magic to produce an identifier temporary.
// identifiers after all are glorified pointers.


// SUMMARY :: convert from `Id` to any other expected type in an expression is done by a read access.
// `@` op takes an `Id` and pulls out the pointer.
// `^` op takes a pointer and creates an `Id` temporary.
//
// all in all, seems like we can avoid this whole concept of lvalue, rvalue, references, etc.
// and instead we get something that is arguably clean.
