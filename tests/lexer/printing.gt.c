void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.
    char *str;
    char  c;

    uint64_t n;
    
    // pc declaration with import
    tokens.Append(Token(TOKEN_SYMBOL, str="pc", l=1, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=1, bc=4));
    tokens.Append(Token(TOKEN_KEYWORD_IMPORT, l=1, bc=7));
    tokens.Append(Token(TOKEN_QUOTE, str="ppl.console", l=1, bc=15));
    tokens.Append(Token(TOKEN_ENDL, l=1, bc=28));

    // main function declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=2, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=2, bc=6));
    tokens.Append(Token(TOKEN_KEYWORD_FN, l=2, bc=9));
    tokens.Append(Token(TOKEN_PART, c='(', l=2, bc=12));
    tokens.Append(Token(TOKEN_PART, c=')', l=2, bc=13));
    tokens.Append(Token(TOKEN_KEYWORD_INT, l=2, bc=16));
    tokens.Append(Token(TOKEN_PART, c='{', l=2, bc=20));

        // Variable d declaration and assignment
        tokens.Append(Token(TOKEN_SYMBOL, str="d", l=3, bc=5));
        tokens.Append(Token(TOKEN_OP_DECL_RUNTIME, l=3, bc=6));
        tokens.Append(Token(TOKEN_KEYWORD_SHORT, l=3, bc=7));
        tokens.Append(Token(TOKEN_OP_ASSIGNMENT, l=3, bc=13));
        tokens.Append(Token(TOKEN_SYMBOL, str="MySumFun", l=3, bc=15));
        tokens.Append(Token(TOKEN_PART, c='(', l=3, bc=23));
            tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=5, l=3, bc=24));
            tokens.Append(Token(TOKEN_OP_COMMA, l=3, bc=25));
            tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=8, l=3, bc=27));
        tokens.Append(Token(TOKEN_PART, c=')', l=3, bc=28));
        tokens.Append(Token(TOKEN_ENDL, l=3, bc=29));

        // pc.print call
        tokens.Append(Token(TOKEN_SYMBOL, str="pc", l=4, bc=5));
        tokens.Append(Token(TOKEN_OP_MEMBER_SELECTION, l=4, bc=7));
        tokens.Append(Token(TOKEN_SYMBOL, str="print", l=4, bc=8));
        tokens.Append(Token(TOKEN_PART, c='(', l=4, bc=13));
            tokens.Append(Token(TOKEN_QUOTE, str="  %d\\n", l=4, bc=14));
            tokens.Append(Token(TOKEN_OP_COMMA, l=4, bc=22));
            tokens.Append(Token(TOKEN_SYMBOL, str="d", l=4, bc=24));
        tokens.Append(Token(TOKEN_PART, c=')', l=4, bc=25));
        tokens.Append(Token(TOKEN_ENDL, l=4, bc=26));

    tokens.Append(Token(TOKEN_PART, c='}', l=5, bc=1));
}
