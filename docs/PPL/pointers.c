// clang-format off
// POINTERS.

// NOTE: int is an alias for int32_t, always.
->int a;

// issue with -> syntax is that a double pointer, or even a triple pointer becomes
// gross. of course, -> increases readability (maybe).

// I think I can agree that one character for pointer is better than two (or any more).
>int a;

// this kind of hits the middle ground between readability and conciseness.
// but it means now that > has two meanings, which is a bit gross.
// in a lang like C++, & has two at least two meanings too (reference and address of).
// of course, one could argue that references and address of are at least related.
//
// but even in C, we have that * means multiply and pointer.
// so it should be okay for us to overlap with a binary operator.

// Pascal:
int  x;
^int a;
a = @x;
^a = 3;

// Decision: I think I like the ^ syntax.
// and we are going to stick to just ONE.
// makes it easy to read code.
// as opposed to allowing the user to use
// whatever they want. Keep it simple.
//
// in pondering > vs ^, ^ is at least consistent visually with * as these take
// up the upper half of the space allocated for a single character.

// Here's another question. On which side of the type do we put the pointer syntax?
^^int vs. int^^ ? ;

// since we are borrowing from Pascal, we ought to stick with how it does things.
^^int; // is correct.

// Question:
// How are we meant to do the two const ideas?
// Where the memory pointed to may not be modified versus the pointer itself cannot be modded?

// HOW QUALIFIERS WORK:
// maybe say that qualifier modifes strictly what is to the right.
// it only modifies just one item.
// a qualifier cannot modify a qualifer, so it searches until it can find the first non qualifer. this allows for qualifier sequences.

// thus,
const int A; 
// modifies direct to its right.
// this becomes, "declare a constant integer called A".
// const sort of folds into the value type.

int const A; // redundant. the identifier A (UID part of UID<T>) is not going to change.
// still allowable but again redundant.
// (unless we are dealing with references).

// if we wanted duck stuff (which I do not see why),
// maybe we use some diff syntax.. so for now, don't care about that.

// so if ...
const ^int A;
// "declare a constant pointer to an integer called A".

// then
^const int A;
// "declaree a pointer to a constant integer called A".


// also, there may be an implicit let,
let const int A;
// "let there be a consant integer called A".
// where the "let there be ..." is an alloc/reserve idea.

// we have to remeember that these decls create a VARIABLE.
// which is some location and an identifier desribing/identifying it.

int32_t main(int32_t argc, ^^uint8_t argv) {
}
