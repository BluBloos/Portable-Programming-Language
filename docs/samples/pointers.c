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
int x;
^int a;
a = @x;
^a = 3;

// Decision: I think I like the ^ syntax.
// and we are going to stick to just ONE.
// makes it easy to read code.
// as opposed to allowing the user to use
// whatever they want. Keep it simple.

int32_t main(int32_t argc, ^^uint8_t argv)
{
    
}