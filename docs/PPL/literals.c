int main() {
    unsigned char a = 0b01010101;  // binary literals
    uint8_t       b = 0xFF;        // Hexadecimal literals
    float         c = 1.0e9;       // scientific notation

    // ---------- STRING STUFF ----------
    // UTF-8 is the default encoding.

    // TODO: maybe add other encodings?

    ^char d = "Hello, world!";  // null-terminated 0-127 ASCII string literal.
    // the above is also an UTF-8 string literal. recall that UTF-8 decays to
    // ASCII when the string is 0-127.
    uint32_t e = '😂';         // utf-8 literal.
    ^char8_t f = "Hello 🧠!";  // again, all strings are UTF-8.
    // and we are also borrowing char8_t from C++20.
    char e = 'a';  // character literal

    int bigNumber = 1_000_000;  // digit separators for big numbers :D
}