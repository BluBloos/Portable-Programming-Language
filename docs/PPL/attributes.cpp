// we use the pack attribute to ensure that the compiler will pack this struct
// in memory, ignoring the performance implications of this.
S : [[pack]] struct {
    a : int;
    b : char;
};