// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    tree_node mul = CreateTree(AST_OP_MULTIPLICATION);
    char *s;
    auto lhs = CreateTree(AST_SYMBOL, s="a");
    auto rhs = CreateTree(AST_SYMBOL, s="b");
    TreeAdoptTree(mul, lhs);
    TreeAdoptTree(mul, rhs);

    tree_node plus1 = CreateTree(AST_OP_ADDITION);
    rhs = CreateTree(AST_SYMBOL, s="c");
    TreeAdoptTree(plus1, mul);
    TreeAdoptTree(plus1, rhs);

    tree_node lt = CreateTree(AST_OP_LESS_THAN);
    rhs = CreateTree(AST_SYMBOL, s="d");
    TreeAdoptTree(lt, plus1);
    TreeAdoptTree(lt, rhs);

    return lt;
}