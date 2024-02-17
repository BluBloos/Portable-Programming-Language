// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    int64_t i;
    char *s;
    tree_node a = CreateTree(AST_OP_BITWISE_XOR_ASSIGNMENT);
    auto lhs = CreateTree(AST_SYMBOL, s="a");
    TreeAdoptTree(a, lhs);
    
        tree_node o = CreateTree(AST_OP_BITWISE_OR);

            tree_node g = CreateTree(AST_OP_MEMBER_SELECTION);
            lhs = CreateTree(AST_SYMBOL, s="epicVar");
            auto rhs = CreateTree(AST_SYMBOL, s="hi");
            TreeAdoptTree(g, lhs);
            TreeAdoptTree(g, rhs);

            tree_node ba = CreateTree(AST_OP_BITWISE_AND);
            lhs = CreateTree(AST_INT_LITERAL,i=7);
            rhs = CreateTree(AST_INT_LITERAL,i=3);
            TreeAdoptTree(ba, lhs);
            TreeAdoptTree(ba, rhs);
        
        TreeAdoptTree(o, g);
        TreeAdoptTree(o, ba);

    TreeAdoptTree(a, o);

    return a;
}
