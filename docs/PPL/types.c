int main(int argc, >> char argv)
{
    a : int;    // always 32 bits.
    b : char;   // always 8 bits.
    c : short;  // always 16 bits.
    // ^ default is signed.

    e : unsigned int;  // now unsigned 32 bits.

    // bool is a type alias to ui8:1.
    d : bool;

    // long is not supported.

    f : ui32;     // these sorts of things exist for all of (u) vs. (no u) and
                  // any 8 -> 64 sizes. these are integer types.

    i : uint32_t; // but we also have these types from stdint.h as well.

    // TODO: would we want 128 size support?

    float  g;
    double h;
}