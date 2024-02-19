void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.

    uint64_t n;

    char *str;
    char  c;
    
    // Import statement
    tokens.Append(Token(TOKEN_KEYWORD_IMPORT, l=3, bc=1));
    tokens.Append(Token(TOKEN_QUOTE, str="ppl.console", l=3, bc=9));
    tokens.Append(Token(TOKEN_ENDL, l=3, bc=22));

    // fib function
    tokens.Append(Token(TOKEN_SYMBOL, str="fib", l=5, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=5, bc=4)); // "::"
    tokens.Append(Token(TOKEN_KEYWORD_FN, l=5, bc=6));
    tokens.Append(Token(TOKEN_PART, c='(', l=5, bc=8));
        tokens.Append(Token(TOKEN_SYMBOL, str="n", l=5, bc=9));
        tokens.Append(Token(TOKEN_OP_DECL_RUNTIME, l=5, bc=10));
        tokens.Append(Token(TOKEN_KEYWORD_INT, l=5, bc=11));
    tokens.Append(Token(TOKEN_PART, c=')', l=5, bc=14));
    tokens.Append(Token(TOKEN_KEYWORD_INT, l=5, bc=15));
    tokens.Append(Token(TOKEN_PART, c='{', l=5, bc=19));
        // if statement
        tokens.Append(Token(TOKEN_KEYWORD_IF, l=6, bc=3));
        tokens.Append(Token(TOKEN_SYMBOL, str="n", l=6, bc=6));
        tokens.Append(Token(TOKEN_OP_LESS_THAN_OR_EQUAL_TO,l=6, bc=8));
        tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=1, l=6, bc=11));
        // return statement for if
        tokens.Append(Token(TOKEN_KEYWORD_RETURN, l=7, bc=5));
        tokens.Append(Token(TOKEN_SYMBOL, str="n", l=7, bc=12));
        tokens.Append(Token(TOKEN_ENDL, l=7, bc=13));
        // return statement for fib function
        tokens.Append(Token(TOKEN_KEYWORD_RETURN, l=8, bc=3));
        tokens.Append(Token(TOKEN_SYMBOL, str="fib", l=8, bc=10));
        tokens.Append(Token(TOKEN_PART, c='(', l=8, bc=13));
            tokens.Append(Token(TOKEN_SYMBOL, str="n", l=8, bc=14));
            tokens.Append(Token(TOKEN_OP_SUBTRACTION, l=8, bc=16));
            tokens.Append(Token(TOKEN_INTEGER_LITERAL,n=1, l=8, bc=18));
        tokens.Append(Token(TOKEN_PART, c=')', l=8, bc=19));
        tokens.Append(Token(TOKEN_OP_ADDITION, l=8, bc=21));
        tokens.Append(Token(TOKEN_SYMBOL, str="fib", l=8, bc=23));
        tokens.Append(Token(TOKEN_PART, c='(', l=8, bc=26));
            tokens.Append(Token(TOKEN_SYMBOL, str="n", l=8, bc=27));
            tokens.Append(Token(TOKEN_OP_SUBTRACTION, l=8, bc=29));
            tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=2, l=8, bc=31));
        tokens.Append(Token(TOKEN_PART, c=')', l=8, bc=32));
        tokens.Append(Token(TOKEN_ENDL, l=8, bc=33));
    tokens.Append(Token(TOKEN_PART, c='}', l=9, bc=1));

    // main function
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=10, bc=1));
    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=10, bc=5)); // "::"
    tokens.Append(Token(TOKEN_KEYWORD_FN, l=10, bc=7));
    tokens.Append(Token(TOKEN_PART, c='(', l=10, bc=9));
    tokens.Append(Token(TOKEN_PART, c=')', l=10, bc=10));
    tokens.Append(Token(TOKEN_KEYWORD_INT, l=10, bc=11));
    tokens.Append(Token(TOKEN_PART, c='{', l=10, bc=15));
        // print call in main function
        tokens.Append(Token(TOKEN_SYMBOL, str="print", l=11, bc=3));
        tokens.Append(Token(TOKEN_PART, c='(', l=11, bc=8));
            tokens.Append(Token(TOKEN_QUOTE, str="Fib(7)=%d", l=11, bc=9));
            tokens.Append(Token(TOKEN_OP_COMMA, l=11, bc=20));
            tokens.Append(Token(TOKEN_SYMBOL, str="fib", l=11, bc=22));
            tokens.Append(Token(TOKEN_PART, c='(', l=11, bc=25));
                tokens.Append(Token(TOKEN_INTEGER_LITERAL, n=7, l=11, bc=26));
            tokens.Append(Token(TOKEN_PART, c=')', l=11, bc=27));
        tokens.Append(Token(TOKEN_PART, c=')', l=11, bc=28));
    tokens.Append(Token(TOKEN_ENDL, l=11, bc=29));
    tokens.Append(Token(TOKEN_PART, c='}', l=12, bc=1));
}
