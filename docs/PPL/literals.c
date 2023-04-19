int main() {
    a : unsigned char  = 0b01010101;  // binary literals
    b : uint8_t = 0xFF;        // Hexadecimal literals
    c : float = 1.0e9;       // scientific notation

    // ---------- STRING STUFF ----------
    // UTF-8 is the default encoding.

    // TODO: maybe add other encodings?

    d : ^char = "Hello, world!";  // null-terminated 0-127 ASCII string literal.
    // the above is also an UTF-8 string literal. recall that UTF-8 decays to
    // ASCII when the string is 0-127.
    e : uint32_t = '😂';         // utf-8 literal.
    f : ^char8_t = "Hello 🧠!";  // again, all strings are UTF-8.
    // and we are also borrowing char8_t from C++20.
    e : char = 'a';  // character literal

    bigNumber : int = 1_000_000;  // digit separators for big numbers :D
}