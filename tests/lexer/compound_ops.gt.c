// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground truth files; the ground truth
// gen functions need to have different names.

void GENERATE_GROUND_TRUTH(TokenContainer &tokens)
{
    uint32_t bc = 1; // begin column.
    uint64 l;
    
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=6, 1, bc));
    tokens.Append(Token(TOKEN_OP_LOGICAL_AND,   1, bc=3));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=7, 1, bc=6));
    
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=7, 2, bc=1));
    // TODO: quite silly here when making the bitwise_and plus other
    // token kinds. should be standard way to do that.
    tokens.Append(Token(TOKEN_OP_BITWISE_AND,   2, bc=3));
    tokens.Append(Token(TOKEN_OP_BITWISE_AND,   2, bc=5));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=8, 2, bc=9));

    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=9, 3, bc=1));
    tokens.Append(Token(TOKEN_OP_BITWISE_AND,   3, bc=3));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=8, 3, bc=5));

    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=0, 5, bc=1));
    tokens.Append(Token(TOKEN_OP_SPAN_CTOR,  5, bc=3));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=1, 5, bc=7));

    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=0, 7, bc=1));
    tokens.Append(Token(TOKEN_OP_SPAN_CTOR_STRICT,   7, bc=3));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=1, 7, bc=7));

    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=1, 8, bc=1));
    tokens.Append(Token(TOKEN_OP_BITWISE_LEFT_SHIFT,   8, bc=3));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=2, 8, bc=5));
    tokens.Append(Token(TOKEN_OP_BITWISE_RIGHT_SHIFT, 8, bc=6));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=3, 8, bc=8));
    tokens.Append(Token(TOKEN_OP_LEFT_SHIFT_ASSIGNMENT,   8, bc=9));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=4, 8, bc=12));
    tokens.Append(Token(TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT,   8, bc=13));
    tokens.Append(Token(TOKEN_INTEGER_LITERAL, l=5, 8, bc=16));
}