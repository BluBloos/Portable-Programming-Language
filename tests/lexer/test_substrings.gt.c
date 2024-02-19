void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.
    char *str;
    char  c;

    uint64_t n;
    
    // input_off declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="input_off", l=1, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=1, bc=11));
    tokens.Append(Token(TOKEN_PART, c='{', l=1, bc=14));
        // integer declaration
        tokens.Append(Token(TOKEN_SYMBOL, str="integer", l=2, bc=5));
        tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=2, bc=13));
        tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=1, l=2, bc=16));
        tokens.Append(Token(TOKEN_ENDL, l=2, bc=17));
        // return statement
        tokens.Append(Token(TOKEN_KEYWORD_RETURN, l=3, bc=5));
        tokens.Append(Token(TOKEN_ENDL, l=3, bc=11));
    tokens.Append(Token(TOKEN_PART, c='}', l=4, bc=1));

    // main function declaration
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=6, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_RUNTIME_INFER, l=6, bc=6));
    tokens.Append(Token(TOKEN_PART, c='(', l=6, bc=9));
    tokens.Append(Token(TOKEN_PART, c=')', l=6, bc=10));
    tokens.Append(Token(TOKEN_KEYWORD_INT, l=6, bc=13));
    tokens.Append(Token(TOKEN_PART, c='{', l=6, bc=17));
        // print call
        tokens.Append(Token(TOKEN_SYMBOL, str="print", l=7, bc=5));
        tokens.Append(Token(TOKEN_PART, c='(', l=7, bc=10));
            tokens.Append(Token(TOKEN_QUOTE, str="hi", l=7, bc=11));
        tokens.Append(Token(TOKEN_PART, c=')', l=7, bc=15));
        tokens.Append(Token(TOKEN_ENDL, l=7, bc=16));
    tokens.Append(Token(TOKEN_PART, c='}', l=8, bc=1));
}
