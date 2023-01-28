int main(int argc, >> char argv) {
    int   a;  // always 32 bits.
    char  a;  // always 8 bits.
    short a;  // always 16 bits.
    bool  a;  // always 1 bit. So _can_ be packed in struct as 1 bit. Its _can_ because for perf default on struct is no
              // pack and align for target.
    // ^ default is signed.
    unsigned int a;  // now uint32_t.

    // support for "long" does not exist. it's just odd.
    // but #idea we could supply (on user request, and commuicate how to request on error of long not being defined)
    // that we desire "C99 mode", or something. It would functionally turn the PPL lang into C99 in the set of interop
    // langs.

    uint32_t a;  // these sorts of things exist for all of (u) vs. (no u) and any 8 -> 64 sizes.
    // TODO: would we want 128 size support?

    float  a;
    double b;
}