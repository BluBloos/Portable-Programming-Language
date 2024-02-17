// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    int64_t i;
    tree_node tc = CreateTree(AST_OP_TERNARY_CONDITIONAL);
    auto tb = CreateTree(AST_INT_LITERAL, i=4);
    auto fb = CreateTree(AST_INT_LITERAL, i=3);
    
        tree_node cond = CreateTree(AST_OP_MODULUS);
        auto lhs = CreateTree(AST_INT_LITERAL, i=54);
        auto rhs = CreateTree(AST_INT_LITERAL, i=8);
        TreeAdoptTree(cond, lhs);
        TreeAdoptTree(cond, rhs);

    TreeAdoptTree(tc, cond);
    TreeAdoptTree(tc, tb);
    TreeAdoptTree(tc, fb);

    return tc;
}
