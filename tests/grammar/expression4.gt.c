// .gt for the "ground truth".

// NOTE: this .c file will be #include during the building of the
// "test version" of the PPL compiler toolchain. GENERATE_GROUND_TRUTH is
// a macro whose name will be set before including this .c file.
// this happens because there are many such ground 

tree_node GENERATE_GROUND_TRUTH()
{
    char *s;

    tree_node lt = CreateTree(AST_OP_LESS_THAN);    
    auto lhs = CreateTree(AST_SYMBOL, s="a");
    auto rhs = CreateTree(AST_SYMBOL, s="b");
    TreeAdoptTree(lt, lhs);
    
        tree_node plus = CreateTree(AST_OP_ADDITION);
        lhs = CreateTree(AST_SYMBOL, s="b");
        TreeAdoptTree(plus, lhs);

            tree_node mul = CreateTree(AST_OP_MULTIPLICATION);
            lhs = CreateTree(AST_SYMBOL, s="c");
            rhs = CreateTree(AST_SYMBOL, s="d");
            TreeAdoptTree(mul, lhs);
            TreeAdoptTree(mul, rhs);

        TreeAdoptTree(plus, mul);

    TreeAdoptTree(lt, plus);

    return lt;
}