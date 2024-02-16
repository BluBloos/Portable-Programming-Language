// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    tree_node plus = CreateTree(AST_OP_ADDITION);

    int64_t i;

    auto lhs = CreateTree(AST_INT_LITERAL, i=1);
    auto rhs = CreateTree(AST_INT_LITERAL, i=3);

    TreeAdoptTree(plus, lhs);
    TreeAdoptTree(plus, rhs);

    return plus;
}