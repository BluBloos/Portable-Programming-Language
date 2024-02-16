// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    // TODO: the tree that we say is correct here doesn't feel right?
    // is this the same way that C would do it?

    tree_node mul = CreateTree(AST_OP_MULTIPLICATION);
    char *s;
    auto lhs = CreateTree(AST_SYMBOL, s="c");
    auto rhs = CreateTree(AST_SYMBOL, s="d");
    TreeAdoptTree(mul, lhs);
    TreeAdoptTree(mul, rhs);

    tree_node plus1 = CreateTree(AST_OP_ADDITION);
    rhs = CreateTree(AST_SYMBOL, s="e");
    TreeAdoptTree(plus1, mul);
    TreeAdoptTree(plus1, rhs);

    tree_node plus2 = CreateTree(AST_OP_ADDITION);
    lhs = CreateTree(AST_SYMBOL, s="b");
    TreeAdoptTree(plus2, lhs);
    TreeAdoptTree(plus2, plus1);

    tree_node gt = CreateTree(AST_OP_GREATER_THAN);
    lhs = CreateTree(AST_SYMBOL, s="a");
    TreeAdoptTree(gt, lhs);
    TreeAdoptTree(gt, plus2);

    return gt;
}