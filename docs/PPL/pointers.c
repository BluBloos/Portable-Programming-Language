// -------------- POINTERS --------------

// I'm not quite sure what syntax to use for pointers.
// below are some ideas.

// idea 1.
->int a;

// the issue with '->' syntax is that a double pointer, or even a triple pointer
// becomes gross. maybe '->' increases readability?

// I think I can agree that one character is better than two (or any more).
// idea 2.
// clang-format off
>int a;
// clang-format on

// this kind of hits the middle ground between readability and conciseness.

// here's another one from Pascal.
// idea 3.
int  x;
^int a;
a  = @x;
^a = 3;

// Decision:
// I think I like the ^ syntax.

// in pondering '>' vs Pascal, the '^' is at least consistent visually with the
// C/C++ '*'. these take up the upper half of the space allocated for a single
// character.

// Here's another question. On which side of the type do we put the pointer
// syntax?
^^int a;
// OR
// clang-format off
int^^ a;
// clang-format on

// since we are borrowing from Pascal, we aught to stick with how it does
// things.
^^int;  // is correct.

// Question:
// How are we meant to do the two const ideas?
// where
// idea 1: the memory pointed to may not be modified.
// idea 2: the pointer itself cannot be modded.

// clang-format off
const ^int A;
// clang-format on
// "declare a constant pointer to an integer called A".

// then
^const int A;
// "declare a pointer to a constant integer called A".

// please see qualifiers.c for more info on const.

// also, I'm thinking we could do an implicit let,
let const int A;
// "let there be a consant integer called A".
// where the "let there be ..." is an alloc/reserve idea.

int32_t main(int32_t argc, ^^uint8_t argv) {}
