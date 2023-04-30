// TODO(Noah):
// What if, to make the codegen part of things easier, we went ahead and added 'labels' to components
// or children of grammer objects???
// if I parse an if-statement,
//     I might have component A (the expression)
//     and component B (the body of the if statement)
// and maybe I can change the syntax of the if-statment all that I please (on the grammer end of things)
// but the AST that pops out is annotated, to make finding A and B easy. Let's call this
// "linking" with A and B. 

// TODO(Noah): Really need to check if there might be some memory-leaks going on here....
// I am doing some FuNkY things with the trees and so forth...

struct ast_error {
    char *msg;
    unsigned int lineNumber;
    unsigned int tokenIndex;
};

struct ast_error CreateASTError(char *msg, unsigned int lineNumber, unsigned int tokenIndex) {
    struct ast_error a = {};
    a.msg = msg; a.lineNumber = lineNumber; a.tokenIndex = tokenIndex;
    return a;
}

void PrintAstError(struct ast_error err) {
    LOGGER.Error("Syntax error on line %d. %s", err.lineNumber, err.msg);
}

bool ParseTokensWithGrammer(
    TokenContainer &tokens, 
    struct grammer_definition grammerDef,
    struct tree_node &tree);

bool ParseTokensWithRegexTree(
    TokenContainer &tokens, 
    struct tree_node regexTree,
    struct tree_node &tree) 
{
    
    //treesParsed = [];
    //buffered_errors = [] //# Frames for each recursive func call.
    
    

    // NOTE(Noah): Was going to make a ParseWithRegexTreeAny
    // function. But this is just same func with knowledge of parent.
    // Easy, we just check if regexTree is an Any.
    bool any_flag = regexTree.type == TREE_REGEX_ANY;
    bool group_flag = regexTree.type == TREE_REGEX_GROUP;
    unsigned int k = 0;

    bool global_fail_flag = (any_flag) ? true : false;

    // Presuming that we are inside of an Any block, we want to preserve state.
    // to rollback tokens upon failure.
    // NOTE(Noah): We also want to rollback tokens when we tried to parse a group with a modifier
    // on the group.
    
    int tokens_savepoint = -1; // default index is invalid.
    if (any_flag)
        tokens_savepoint = tokens.GetSavepoint();

    while ( k < regexTree.childrenCount ) {

        if (group_flag) {
            // clear errors from last k.
            // buffered_errors = []
        }

        struct tree_node &child = regexTree.children[k];
        char modifier = child.metadata.regex_mod; // mod = 0 is a NULL modifier.
        enum tree_type childType = child.type;

        // None means 1 and exactly 1
        // ? is the 0 or 1 modifier
        // * is the 0 or many modifier
        // + is the 1 or many modifier
        // 0 means there was no modifier. We match 1 and exactly 1.
        int n = 0;
        bool infWhile = ( modifier == '*' || modifier == '+' ); 
        int re_matched = 0;

        int premature_n = 0; // this variable is how many times we enter the while loop below.
        int stable_n_checkpoint = tokens.GetSavepoint();

        while ( infWhile ? true : n < 1 ) {
            premature_n += 1;

            if (childType == TREE_REGEX_GROUP || childType == TREE_REGEX_ANY) {
                // TODO(Noah): Check if there is anything smarter to do than
                // "dummyTree"
                struct tree_node dummyTree = CreateTree(TREE_ROOT);
                if (ParseTokensWithRegexTree(tokens, child, dummyTree)) {
                    re_matched += 1;
                    for (unsigned int i = 0; i < dummyTree.childrenCount; i++) {
                        TreeAdoptTree(tree, dummyTree.children[i]);
                    }
                } else {
                    // buffered_errors += (_buffered_errors);
                    DeallocTree(dummyTree);  
                    break;
                } 
            } 
            else if (childType == TREE_REGEX_KEYWORD) {
                char *child_data = child.metadata.str; 
                struct token tok = tokens.QueryNext();
                // TODO(Noah): This is gross. Fix it, you lazy fuck.
                if (tok.type == TOKEN_KEYWORD && 
                    strlen(child_data) == strlen(tok.str) && 
                    SillyStringStartsWith(child_data, tok.str) )
                {
                    tokens.AdvanceNext();
                    re_matched += 1;
                } else {
                    break;
                }
            }
            else if (childType == TREE_REGEX_CHAR) {
                
                struct token tok = tokens.QueryNext();
                bool didMatch = false;
                switch(tok.type) {
                    case TOKEN_CHARACTER_LITERAL:
                    case TOKEN_OP:
                    case TOKEN_PART:
                    didMatch = (tok.c == child.metadata.c);
                    break;
                    case TOKEN_ENDL:
                    didMatch = child.metadata.c == ';';
                    break;
                    default:
                    didMatch = false;
                    break; // didn't match character
                }
                if (didMatch) {
                    tokens.AdvanceNext();
                    re_matched += 1;
                } else {
                    break;
                }    

            } else if (childType == TREE_REGEX_STR) {

                char *child_data = child.metadata.str;    
                if ( GRAMMER.DefExists(child_data) ) {
                    struct tree_node treeChild;
                    if (ParseTokensWithGrammer(tokens, GRAMMER.defs[child_data], treeChild)) {
                        TreeAdoptTree(tree, treeChild);
                        re_matched += 1;
                    } else {
                        //buffered_errors += (_buffered_errors)
                        break; // didn't find grammer object we wanted.
                    }
                }
                // TODO(opt): the regex representation should not use strings.
                // that is very slow. please we should just use simple enums or something instead of string
                // comparison like this.
                else if ( SillyStringStartsWith(child_data, "literal") ) {
                    struct token tok = tokens.QueryNext(); // # the whole LR k+1 idea :)
                    bool didMatch = true;

                    // TODO: it would be nice if these switch statements could also warn us at compile-time
                    // that we are missing a case. Is there an elegant way in C++ to do this or is this one
                    // of those things that would be best done in the new language?

                    switch (tok.type) {
                        case TOKEN_QUOTE: {
                            tokens.AdvanceNext();
                            struct tree_node newTree = CreateTree(AST_STRING_LITERAL);
                            newTree.metadata.str     = tok.str;
                            TreeAdoptTree(tree, newTree);
                        } break;

                        case TOKEN_DOUBLE_LITERAL: {
                            tokens.AdvanceNext();
                            struct tree_node newTree = CreateTree(AST_DECIMAL_LITERAL, (double)tok.dnum);
                            TreeAdoptTree(tree, newTree);
                        } break;
                        case TOKEN_FLOAT_LITERAL: {
                            // TODO: I know that f64 is more bits than f32, but is there e.g. a loss in precision
                            // when we cast from double to float? some odd floating point representation thing.
                            tokens.AdvanceNext();
                            struct tree_node newTree = CreateTree(AST_DECIMAL_LITERAL, (float)tok.dnum);
                            TreeAdoptTree(tree, newTree);
                        } break;

                        case TOKEN_INTEGER_LITERAL: {
                            tokens.AdvanceNext();
                            
                            // TODO: What happens if we write into the program a value that is too large
                            // to fit into int64_t ?
                            // or in general whenever we are looking at type stuff.
                            // -> this should  be a compiler error about a non-explicit truncation.
                            assert( tok.num > INT64_MAX );

                            struct tree_node newTree = CreateTree(AST_INT_LITERAL, (int64_t)tok.num);
                            TreeAdoptTree(tree, newTree);
                        } break;
                        case TOKEN_CHARACTER_LITERAL: {
                            tokens.AdvanceNext();
                            struct tree_node newTree = CreateTree(AST_CHARACTER_LITERAL, tok.c);
                            TreeAdoptTree(tree, newTree);
                        } break;

                        case TOKEN_NULL_LITERAL: {
                            tokens.AdvanceNext();
                            struct tree_node newTree = CreateTree(AST_NULL_LITERAL);
                            TreeAdoptTree(tree, newTree);
                        } break;

                        default:
                            didMatch = false;
                            break;
                    }

                    if (!didMatch) break;
                    re_matched += 1;

                } else if ( SillyStringStartsWith(child_data, "symbol")) {
                    struct token tok = tokens.QueryNext(); //# the whole LR k+1 idea :)
                    if (tok.type == TOKEN_SYMBOL) {
                        tokens.AdvanceNext();
                        struct tree_node newTree = CreateTree(AST_SYMBOL);
                        newTree.metadata.str = tok.str;
                        TreeAdoptTree(tree, newTree);
                        re_matched += 1;
                    } else {
                        break;
                    }
                } else if ( SillyStringStartsWith(child_data, "op") ) {
                    struct token tok = tokens.QueryNext();
                    bool didMatch = true;

                    // TODO: does the strlen below happen at compile-time?
                    char *op = child_data + strlen("op");

                    // NOTE(Noah): Because I made the delineation between COP and OP, it actually makes
                    // this processing gross-ish.
                    switch(tok.type) {
                        case TOKEN_COP:
                        {
                            // TODO(Noah): This is gross. Fix it, you lazy fuck.
                            didMatch = ( strlen(op) == strlen(tok.str) && 
                                SillyStringStartsWith(op, tok.str));
                        }
                        break;
                        case TOKEN_OP:
                        {
                            // TODO: again, we want to verify that our compile-time strings
                            // such as particular operation types are valid (not negative).
                            // 
                            // I mean, better yet, for perf we should not be using strings at all
                            // because strings are a debug thing.
                            didMatch = (strlen(op) == 1) && ((uint8_t)op[0] == tok.c);
                        }
                        break;
                        default:
                        didMatch = false;
                        break;
                    }
                    if (didMatch) {
                        tokens.AdvanceNext();
                        struct tree_node newTree = CreateTree(AST_OP);
                        newTree.metadata.str = child_data;
                        TreeAdoptTree(tree, newTree);
                        re_matched += 1; 
                    } 
                    else { 
                        break;
                    }

                } else if ( SillyStringStartsWith(child_data, "keyword")) {
                    // NOTE(Noah): So this one is just matching ANY generic keyword.
                    // in which case we want to match like literally whatever we are looking at.
                    // save that information.
                    struct token tok = tokens.QueryNext();
                    if (tok.type == TOKEN_KEYWORD) {
                        tokens.AdvanceNext();
                        struct tree_node newTree = CreateTree(AST_KEYWORD);
                        newTree.metadata.str = tok.str;
                        TreeAdoptTree(tree, newTree);
                    } else {
                        break;
                    }
                    re_matched += 1;
                }
            }

            stable_n_checkpoint = tokens.GetSavepoint();
            n += 1;
        }

        // Find all fail cases.
        bool fail_flag = (modifier == 0 && re_matched != 1) || 
           (modifier == '?' && re_matched > 1) ||
           (modifier == '+' && re_matched == 0);

        if (!fail_flag && premature_n > re_matched) {
            // we attempted to go for more n than were matched, which means
            // that even though we matched enough to fulfill the modifier,
            // we had to fail the last case to learn this.
            tokens.ResetSavepoint(stable_n_checkpoint);
        }
        
        // if we have found the correct option under an Any, no more need to search k.
        // if we have failed and are not under an any, also no more need to searh k.
        if (fail_flag && !any_flag) {
            global_fail_flag = true;
            break;
        } else if (any_flag && !fail_flag) {
            global_fail_flag = false;
            break;
        }
            
        if (any_flag) {
            // presuming we have not yet succeeded, and we are inside an Any block
            // this means we are trying the next child.
            // we need to reset the state of Tokens.
            tokens.ResetSavepoint(tokens_savepoint);   
        } 

        k += 1;
    
    }

    return !global_fail_flag;
    // return (treesParsed, fail_flag, buffered_errors)
}

/*
    Returns a tree where root is simply the grammer.name

    Suppose regex = r"[(function_decl)(function_impl)(var_decl)(struct_decl)]*"
    - We can see that the top level is a list given by the *.
    - So we make many an array of children under the root node, one for each of these sub grammer objects.

    Suppose regex = r"(type)(symbol)\(((lv),)*(lv)?\)[;(block)]"
    - first child is the type
    - next child is the symbol
    - then we have another child which is a list of the ((lv),) ones. We know to make a list child because of *
    - then we have another list child for the (lv) denoted by ?. This will be either empty or contain one elem.
    - then we have a child that may or may not exist, which is the block element.

    Suppose regex = 

    Thus far the tree making algo is as follows. Simply go left to right through a regex, 
    Add a child for each grammer element.
    If there is a group, this creates an empty root under which there may be grammer element children.
    If there is a * or ?, or any of the sort, we now have a list child that contains many tree roots.
        - These tree roots may be single grammer element roots or empty group roots.
    A nice prune is to remove empty group parents when there is only 1 child (keeps the tree simpler).
*/

bool ParseTokensWithGrammer(
    TokenContainer &tokens, 
    struct grammer_definition grammerDef,
    struct tree_node &tree) {
    
    if (!GRAMMER.DefExists(grammerDef.name)) return false;

    //buffered_errors = []
    
    // NOTE(Noah): Here we do not alloc another string.
    // The string has already been secured and alloced inside of grammerDef.
    tree = CreateTree(AST_GNODE);
    tree.metadata.str = (char *)grammerDef.name;
    
    // Fill up the tree with any parsed children.
    bool r = ParseTokensWithRegexTree(tokens, 
        grammerDef.regexTree, tree);
    
    //if ( SillyStringStartsWith(grammerDef.name, "program") )
        //buffered_errors += (_buffered_errors);
    
    // NOTE(Noah): I know this is going to work and do what I want it to do, but 
    // I would say that there MUST be a better way to do this...
    // Namely I do not like len(trees) == 1...
    
    /*
    if ( fail_flag && len(trees) == 1 ) {
        if (GRAMMER.DefExists(grammerDef.name)) {
            for eq in g.equivalences[grammerDef.name]:
                if eq == trees[0].data:
                    fail_flag = False # redemption.
                    return (trees[0], buffered_errors) # override upper level grammer object. Makes AST simpler.
            
        }
    }
    */

    if (!r) {
        // Delete any children that might have been created.
        for (unsigned int i = 0; i < tree.childrenCount; i++) {
            DeallocTree(tree.children[i]);
        }
        tree.childrenCount = 0; 
        return false;
    } 
    return true;
}
   

/*
def Run(tokens, logger):
    grammer = g.LoadGrammer()
    abstractSyntaxTree, bufErrors = ParseTokensWithGrammer(tokens, grammer, grammer.defs["program"], logger)
    # check if it's valid.
    if abstractSyntaxTree and tokens.QueryNext().type == "EOL":
        return abstractSyntaxTree
    else:
        # NOTE(Noah): This is extra slow code. Doing a for loop twice for NO reason. Just easier to write out...
        tokenIndex = max(error.token_index for error in bufErrors)
        for error in bufErrors:
            if error.token_index == tokenIndex:
                logger.Error(str(error))
    return False
*/