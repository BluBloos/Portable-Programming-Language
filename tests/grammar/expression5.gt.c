// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    tree_node bo = CreateTree(AST_OP_LOGICAL_OR);    
    
        tree_node a = CreateTree(AST_OP_LOGICAL_AND);
        auto lhs = CreateTree(AST_INT_LITERAL, true);
        auto rhs = CreateTree(AST_INT_LITERAL, false);
        TreeAdoptTree(a, lhs);
        TreeAdoptTree(a, rhs);
        
        int64_t i;
        tree_node n3 = CreateTree(AST_OP_LOGICAL_NOT);
        auto thing = CreateTree(AST_INT_LITERAL, i=3);
        TreeAdoptTree(n3, thing);

    TreeAdoptTree(bo, a);
    TreeAdoptTree(bo, n3);

    return bo;
}
