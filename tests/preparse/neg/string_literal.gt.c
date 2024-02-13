void GENERATE_GROUND_TRUTH(TokenContainer &tokens, ppl_error_context &ctx)
{
    // no tokens with this test, just error.
    ctx.c = 13;
    ctx.line =2;
    ctx.kind = PPL_ERROR_KIND_PARSER;

    strcpy(ctx.errMsg, "Unclosed string literal. Began at 2,13.");
}
