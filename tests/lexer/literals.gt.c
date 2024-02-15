void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.
    char *str;
    char  c;
    uint64_t n;
    double d;;
    float f;
    
    // main function declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=1, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=1, bc=6));
    tokens.Append(Token(TOKEN_KEYWORD, str="fn", l=1, bc=9));
    tokens.Append(Token(TOKEN_PART, c='(', l=1, bc=11));
    tokens.Append(Token(TOKEN_PART, c=')', l=1, bc=12));
    tokens.Append(Token(TOKEN_KEYWORD, str="int", l=1, bc=14));
    tokens.Append(Token(TOKEN_PART, c='{', l=1, bc=18));
    
    // Variable x declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="x", l=2, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=2, bc=7));
    tokens.Append(Token(TOKEN_FLOAT_LITERAL, f=10.5f, l=2, bc=10));
    tokens.Append(Token(TOKEN_ENDL, l=2, bc=15));

    // Variable y declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="y", l=3, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER,  l=3, bc=7));
    tokens.Append(Token(TOKEN_DOUBLE_LITERAL, d=10.0, l=3, bc=10));
    tokens.Append(Token(TOKEN_ENDL, l=3, bc=14));

    // Variable z declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="z", l=4, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=4, bc=7));
    tokens.Append(Token(TOKEN_UINT_LITERAL,n=0, l=4, bc=10));
    tokens.Append(Token(TOKEN_ENDL, l=4, bc=12));

    // Variable w declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="w", l=5, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=5, bc=7));
    tokens.Append(Token(TOKEN_TRUE_LITERAL,  l=5, bc=12));
    tokens.Append(Token(TOKEN_ENDL, l=5, bc=16));

    // Variable h declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="h", l=6, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=6, bc=7));
    tokens.Append(Token(TOKEN_FALSE_LITERAL,   l=6, bc=10));
    tokens.Append(Token(TOKEN_ENDL, l=6, bc=15));

    // Variable hi declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="hi", l=7, bc=5));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=7, bc=7));
    tokens.Append(Token(TOKEN_NULL_LITERAL,   l=7, bc=9));
    tokens.Append(Token(TOKEN_ENDL, l=7, bc=13));

    tokens.Append(Token(TOKEN_PART, c='}', l=8, bc=1));
}
