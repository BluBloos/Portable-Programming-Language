void GENERATE_GROUND_TRUTH(TokenContainer &tokens, ppl_error_context &ctx)
{
    // no tokens with this test, just error.
    ctx.c = 1;
    ctx.line =1;
    ctx.kind = PPL_ERROR_KIND_PARSER;

    strcpy(ctx.errMsg, "Unclosed multiline comment. Began at 1,1.");
}
