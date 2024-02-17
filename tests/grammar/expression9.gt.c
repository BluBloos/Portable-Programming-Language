// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    int64_t i;
    char *s;
    tree_node t = CreateTree(AST_OP_LOGICAL_OR);

    ppl_type type;
    
    auto lhs = CreateTree(AST_TYPE_LITERAL, type=PPL_TYPE_S64);
    auto rhs = CreateTree(AST_TYPE_LITERAL, type=PPL_TYPE_BOOL);

    TreeAdoptTree(t, lhs);
    TreeAdoptTree(t, rhs);

    return t;
}
