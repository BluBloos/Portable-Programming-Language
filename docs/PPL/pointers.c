// -------------- POINTERS --------------

// I'm not quite sure what syntax to use for pointers.
// below are some ideas.

// idea 1.
a : ->int;

// the issue with '->' syntax is that a double pointer, or even a triple pointer
// becomes gross. maybe '->' increases readability?

// I think I can agree that one character is better than two (or any more).
// idea 2.
// clang-format off
a : >int ;
// clang-format on

// this kind of hits the middle ground between readability and conciseness.

// here's another one from Pascal.
// idea 3.
x : int;
a : ^int;
a = @x;
^a = 3;

// Decision:
// I think I like the ^ syntax.

// in pondering '>' vs Pascal, the '^' is at least consistent visually with the
// C/C++ '*'. these take up the upper half of the space allocated for a single
// character.

// Here's another question. On which side of the type do we put the pointer
// syntax?
a : ^^int;
// OR
// clang-format off
a : int^^;
// clang-format on

// since we are borrowing from Pascal, we ought to stick with how it does
// things.
^^int;  // is correct.

// Question:
// How are we meant to do the two const ideas?
// where
// idea 1: the memory pointed to may not be modified.
// idea 2: the pointer itself cannot be modded.

A : const ^int; // "let A be a constant pointer to an integer".
A : ^const int; // "let A be a pointer to a constant integer".

// please see qualifiers.c for more info on const.


main : (argc : int32_t, argv : ^^uint8_t) -> u32 {

}
