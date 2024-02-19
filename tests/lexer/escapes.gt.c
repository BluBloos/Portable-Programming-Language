// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground truth files; the ground truth
// gen functions need to have different names.

void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc; // begin column.
    uint32_t l;  // line.
    char *str;
    char  c;
    
    tokens.Append(Token(TOKEN_SYMBOL, str="main", l=1, bc=1));

    tokens.Append(Token(TOKEN_OP_DECL_COMPILER, l=1, bc=6)); // "::"
   
    tokens.Append(Token(TOKEN_KEYWORD_FN, l=3, bc=1));
    
    // NOTE: I made this decision because I don't like that I was parsing
    // the parens as the same token. it can't be a token op 
    tokens.Append(Token(TOKEN_PART, c='(', l=3, bc=3));
    tokens.Append(Token(TOKEN_PART, c=')', l=3, bc=5));

    tokens.Append(Token(TOKEN_KEYWORD_INT, l=3, bc=8));

    tokens.Append(Token(TOKEN_PART, c='{', l=3, bc=14));
        tokens.Append(Token(TOKEN_SYMBOL, str="printf", l=7, bc=5));
        tokens.Append(Token(TOKEN_PART, c='(', l=7, bc=11));
            tokens.Append(Token(TOKEN_QUOTE, str="\"Hello, World!\"\\n", l=7, bc=12));
        tokens.Append(Token(TOKEN_PART, c=')', l=7, bc=33));
    tokens.Append(Token(TOKEN_ENDL,l=7, bc=34));
    tokens.Append(Token(TOKEN_PART, c='}', l=8, bc=1));
}