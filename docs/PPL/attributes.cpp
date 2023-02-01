[[nodiscard]] // same as nodiscard in C++17
int f() {
    return 0;
}

// we use the pack attribute to ensure that the compiler will pack this struct
// in memory, ignoring the performance implications of this.
[[pack]] struct S {
    int  a;
    char b;
};