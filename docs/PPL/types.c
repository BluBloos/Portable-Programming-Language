int main(int argc, >> char argv) {
    int   a;  // always 32 bits.
    char  b;  // always 8 bits.
    short c;  // always 16 bits.
    // ^ default is signed.
    unsigned int e;  // now uint32_t.

    // always 1 bit. So _can_ be packed in struct as 1 bit. It's "_can_" because
    // for perf, default on struct is no pack and align for target. So bool is
    // likely to take 8 bits.
    bool d;

    // Support for "long" does not exist. This is a personal choice.
    // when compiler encounters "long", give warning and communicate how to
    // request C99 mode. C99 mode ON functionally turns the PPL lang into C99
    // from the perspective of the other compile-time interop langs.
    //
    // With this mode, we get back long. we also get a C99 compiler.

    uint32_t f;  // these sorts of things exist for all of (u) vs. (no u) and
                 // any 8 -> 64 sizes.

    // TODO: would we want 128 size support?

    float  g;
    double h;
}