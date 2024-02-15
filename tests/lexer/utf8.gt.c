void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.
    char *str;
    UNICODE_CPOINT  c;

    // First function declaration with Unicode name
    tokens.Append(Token(TOKEN_SYMBOL, str="ひほわれよう", l=1, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=1, bc=8));
    tokens.Append(Token(TOKEN_PART, c='{', l=1, bc=11));
    tokens.Append(Token(TOKEN_PART, c='}', l=2, bc=1));

    // Second function declaration with Unicode name and emoji
    tokens.Append(Token(TOKEN_SYMBOL, str="世丕且且世两上与丑万丣丕且丗丕🥵", l=4, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=4, bc=18));
    tokens.Append(Token(TOKEN_PART, c='(', l=4, bc=21));
    tokens.Append(Token(TOKEN_PART, c=')', l=4, bc=22));
    tokens.Append(Token(TOKEN_OP_BITWISE_XOR, l=4, bc=23));
    tokens.Append(Token(TOKEN_KEYWORD, str="char", l=4, bc=24));
    tokens.Append(Token(TOKEN_PART, c='{', l=4, bc=29));
    tokens.Append(Token(TOKEN_PART, c='}', l=6, bc=1));

    // main function declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=8, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=8, bc=6));
    tokens.Append(Token(TOKEN_PART, c='(', l=8, bc=9));
    tokens.Append(Token(TOKEN_PART, c=')', l=8, bc=10));
    tokens.Append(Token(TOKEN_KEYWORD, str="int", l=8, bc=11));
    tokens.Append(Token(TOKEN_PART, c='{', l=8, bc=15));

        // Variable utf8String declaration and assignment
        tokens.Append(Token(TOKEN_SYMBOL, str="utf8String", l=9, bc=5));
        tokens.Append(Token(TOKEN_OP_DECL_RUNTIME,  l=9, bc=15));
        tokens.Append(Token(TOKEN_OP_BITWISE_XOR, l=9, bc=16));
        tokens.Append(Token(TOKEN_KEYWORD, str="char", l=9, bc=17));
        tokens.Append(Token(TOKEN_OP_ASSIGNMENT, l=9, bc=22));
        tokens.Append(Token(TOKEN_QUOTE, str="🔥🔥🔥🔥🔥🥵", l=9, bc=24));
        tokens.Append(Token(TOKEN_ENDL, l=9, bc=32));

        // Variable myMan declaration and assignment
        tokens.Append(Token(TOKEN_SYMBOL, str="myMan", l=10, bc=5));
        tokens.Append(Token(TOKEN_OP_DECL_RUNTIME, l=10, bc=11));
        tokens.Append(Token(TOKEN_KEYWORD, str="u32", l=10, bc=13));
        tokens.Append(Token(TOKEN_OP_ASSIGNMENT, l=10, bc=17));
        tokens.Append(Token(TOKEN_CHARACTER_LITERAL, c=0x1F631, l=10, bc=19));
        tokens.Append(Token(TOKEN_ENDL, l=10, bc=22));

    tokens.Append(Token(TOKEN_PART, c='}', l=12, bc=1));
}
