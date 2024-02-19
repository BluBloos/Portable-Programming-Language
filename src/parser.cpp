// TODO(Noah): Check if there might be some memory-leaks since we are doing
// wack stuff with trees.

// TODO: in general there are "funny" ideas that need some super fixing.
// 1. there are no error messages if we fail to parse the grammar. that is prob
//    the worst thing ever.
// 2. suppose that you are parsing a program. due to the grammar of the program,
//    it permits that we can have any amount of statements, var_decl, etc.
//    the issue is that if some early ones are valid, and the latter half are not,
//    there are no complaints by the system and it just throws out the invalid part.

// TODO(AST cleanup): there are redundant nodes that
// communicate zero information. the var_decl grammar node definition is an Any
// group. thus, we should never need to have a grammar node that is "var_decl",
// we just require the children grammar kinds i.e. compile/runtime var decl.

// TODO(AST cleanup): the depth of our expression trees is very silly and not
// required at all. we really only need to go some amount up the tree, and after that
// the ancestors do not give an extra info.

#include <algorithm>

//#include <functional>

static char    *g_bufferToPrintTo = nullptr; 
static uint32_t g_bufferToPrintToLen = 0;
static uint32_t g_lastBufPos = 0;

bool ParseTokensWithPrattParser(TokenContainer &tokens,
    const tree_node                            &regexTree,
    const grammar_definition                   &currGrammarCtx,
    tree_node                                  &tree,
    ppl_error_context                          &errorCtx,
    bool                                        parentWantsVerboseAST = false);

bool ParseLiteral(TokenContainer &tokens, tree_node *tree, ppl_error_context &errorCtx);

bool ParseSymbol(TokenContainer &tokens, tree_node *tree, ppl_error_context &errorCtx);

bool ParseOperator(TokenContainer &tokens,
    token_type                     compelled_op,
    tree_node                     *tree,
    ppl_error_context             &errorCtx,
    const grammar_definition      &currGrammarCtx);

// TODO: maybe there is C++ magic to do this is literally any other way.
//       but right now I could not be bothered.
void HackyPrintToBuffer(const char *fmt, ...)
{

    char *where = g_bufferToPrintTo + g_lastBufPos;
    if (g_lastBufPos >= g_bufferToPrintToLen) {
        Assert("TODO.");
        return;
    }

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(
        where, g_bufferToPrintToLen - g_lastBufPos,
        fmt, args
    );

    if (written < 0) {
        Assert("TODO.");
    }
    else
    {
        // NOTE: written is not counting the null terminator.
        g_lastBufPos += written;
    }

    va_end(args);
}


void GenerateCodeContextFromFilePos(ppl_error_context &ctx, uint32_t line, uint32_t c, char *buf, uint32_t bufLen)
{

    const std::string ANSI_RED = "\033[31m";
    const std::string ANSI_RESET = "\033[0m";

    // ReadLine works fast because the lexer phase noted where in the source file it found the `\n`s.
    ppl_str_view strView1 = ctx.pTokenBirthplace->ReadLine(line - 1);  // .line begins at 1.
    // if the index does not map to anything, we get back an empty string view.
    ppl_str_view strView2 = ctx.pTokenBirthplace->ReadLine(line);
    ppl_str_view strView3 = ctx.pTokenBirthplace->ReadLine(line + 1);

    // ReadLine gets the lines with \n OR CP_EOF included.

    std::string firstOne = (strView1.len) ? 
        std::string(strView1.str, strView1.len - 1) + "\n\t" : "";
    std::string secondOne = (strView2.len) ? std::string(strView2.str, strView2.len - 1) + "\n\t" : "";
    std::string thirdOne = (strView3.len) ? std::string(strView3.str, strView3.len - 1) + "\n\t" : "";

    if ( (c > 0) && ((c-1)<secondOne.size()) ) secondOne.insert(c - 1, ANSI_RED);

    // complete insanity below, beware.
    auto result = std::string("\t") + firstOne + secondOne + thirdOne;

    // write out result to buf.
    memcpy(buf, result.c_str(), std::min( (size_t)bufLen, result.size() + 1 )  );
    // the +1 above is for the null-terminator.

}

#if 0
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
#endif

bool ParseTokensWithGrammar(
    TokenContainer &tokens, 
    const grammar_definition &grammarDef,
    struct tree_node *tree,
    ppl_error_context &errorCtx,
    bool parentWantsVerboseAST = false);

bool ParseTokensWithRegexTree(
    TokenContainer &tokens, 
    const tree_node &regexTree,
    const grammar_definition &currGrammarCtx,
    tree_node &tree,
    ppl_error_context &errorCtx,
    bool parentWantsVerboseAST = false)
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

        const tree_node &child = regexTree.children[k];
        enum tree_type childType = child.type;

        // When there is no modifier we match 1 and exactly 1.
        //
        // ? is the 0 or 1 modifier
        // * is the 0 or many modifier
        // + is the 1 or many modifier
        //
        // if the modifier is equal to 0, or is equal to anything other than the three above,
        // that is considered as the "NULL modifier" in the specific class of modifiers above.
        //
        // ` is the "verbose in AST" modifier.
        //   e.g. (keyword=if) will add a node for the if keyword in AST.
        //
        char modifier = child.metadata.regex_mod;

        bool verboseAST = (modifier == '`') || parentWantsVerboseAST; 

        bool nullFrequencyModifier = (modifier != '?') && (modifier != '*') && (modifier != '+');

        int n = 0;
        bool infWhile = ( modifier == '*' || modifier == '+' ); 
        int re_matched = 0;

        int premature_n = 0; // this variable is how many times we enter the while loop below.
        int stable_n_checkpoint = tokens.GetSavepoint();

        while ( infWhile ? true : n < 1 ) {
            premature_n += 1;

            // TODO: why is this not just a switch statement on the childType ??
            
            if (childType == TREE_REGEX_GROUP || childType == TREE_REGEX_ANY) {
                // TODO(Noah): Check if there is anything smarter to do than
                // "dummyTree"
                struct tree_node dummyTree = CreateTree(TREE_ROOT);
                if (ParseTokensWithRegexTree(tokens, child, currGrammarCtx, dummyTree, errorCtx, verboseAST)) {
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
// lmao, we are breaking the whole thing right now.
#if 0
            else if (childType == TREE_REGEX_KEYWORD) {
                const char *child_data = child.metadata.str; 
                struct token tok = tokens.QueryNext();
                // TODO(Noah): This is gross. Fix it, you lazy fuck.
                if (tok.type == TOKEN_KEYWORD && 
                    strlen(child_data) == strlen(tok.str) && 
                    SillyStringStartsWith(child_data, tok.str) )
                {
                    tokens.AdvanceNext();
                    re_matched += 1;

                    if (verboseAST) {
                        struct tree_node newTree = CreateTree(AST_KEYWORD);
                        
                        // TODO: is this safe? this presumes that we never dealloc the tokens and those are required
                        // for a valid AST. OR, we can dealloc tokens, but this works because we have a
                        // separate string store?
                        newTree.metadata.str = tok.str;
                        TreeAdoptTree(tree, newTree);
                    }

                } else {
                    break;
                }
            }
#endif
            else if (childType == TREE_REGEX_CHAR) {
                
                struct token tok = tokens.QueryNext();
                bool didMatch = false;
                switch(tok.type) {
                    case TOKEN_CHARACTER_LITERAL: // TODO: matching char literal here looks wrong.
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
                    if (errorCtx.SubmitError(
                        PPL_ERROR_KIND_PARSER,
                        tok.line, tok.beginCol, tokens.GetSavepoint()
                    )) {
                        snprintf(errorCtx.errMsg, PPL_ERROR_MESSAGE_MAX_LENGTH, "Expected character '%c'.", child.metadata.c);
                    }
                    break;
                }    

            } 
// lmao also breaking this right now.
#if 0
            else if ( childType == TREE_REGEX_GENERIC_KEYWORD ) {
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

                    if (errorCtx.SubmitError(
                        PPL_ERROR_KIND_PARSER,
                        tok.line, tok.beginCol, tokens.GetSavepoint()
                    ))
                    {
                        snprintf(errorCtx.errMsg, PPL_ERROR_MESSAGE_MAX_LENGTH,
                        "Expected a keyword.");
                    }

                    break;
                }
                re_matched += 1;
            }
#endif
            else if ( childType == TREE_REGEX_SYMBOL ) {

                tree_node symbol;
                if (ParseSymbol(tokens, &symbol, errorCtx)) {
                    TreeAdoptTree(tree, symbol);
                    re_matched += 1;
                } else {
                    break;
                }

            }
            else if (childType == TREE_REGEX_LITERAL) {
                
                tree_node literal;
                bool r = ParseLiteral(tokens, &literal, errorCtx);
                if (!r) {
                    break;
                } else {
                    TreeAdoptTree(tree, literal);
                    re_matched +=1;
                }
                
            }
            
            else if (childType == TREE_REGEX_STR) {

                const char *child_data = child.metadata.str;    
                if ( GRAMMAR.DefExists(child_data) ) {

                    struct tree_node treeChild = CreateTree(AST_GNODE);
                    const bool bGrammarDefExists = GRAMMAR.DefExists(child_data);
                    
                    if (bGrammarDefExists)
                    {
                        const grammar_definition &grammarDef = GRAMMAR.defs[child_data];

                        // NOTE(Noah): Here we do not alloc another string.
                        // The string has already been secured and alloced inside of grammarDef.
                        treeChild.metadata.str = grammarDef.name;

                        // Fill up the tree with any parsed children.
                        if (bGrammarDefExists && ParseTokensWithRegexTree(
                                tokens, grammarDef.regexTree, grammarDef, treeChild, errorCtx, verboseAST)) {
                            TreeAdoptTree(tree, treeChild);
                            re_matched += 1;

                        } else {
                            // Delete any children that might have been created.
                            for (unsigned int i = 0; i < treeChild.childrenCount; i++) { DeallocTree(treeChild.children[i]); }
                            treeChild.childrenCount = 0;
                            break;  // didn't find grammar object we wanted.
                        }

                    } else {
                        Assert("TODO(Compiler team): this bad, fix.");
                        break;
                    }

                }
            }
            else
            {
                PPL_TODO;
            }

            stable_n_checkpoint = tokens.GetSavepoint();
            n += 1;
        }

        // Find all fail cases.
        bool fail_flag = ( nullFrequencyModifier && re_matched != 1) || 
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

            Assert( tokens_savepoint >= 0 );

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
    Returns a tree where root is simply the grammar.name

    Suppose regex = r"[(function_decl)(function_impl)(var_decl)(struct_decl)]*"
    - We can see that the top level is a list given by the *.
    - So we make many an array of children under the root node, one for each of these sub grammar objects.

    Suppose regex = r"(type)(symbol)\(((lv),)*(lv)?\)[;(block)]"
    - first child is the type
    - next child is the symbol
    - then we have another child which is a list of the ((lv),) ones. We know to make a list child because of *
    - then we have another list child for the (lv) denoted by ?. This will be either empty or contain one elem.
    - then we have a child that may or may not exist, which is the block element.

    Suppose regex = 

    Thus far the tree making algo is as follows. Simply go left to right through a regex, 
    Add a child for each grammar element.
    If there is a group, this creates an empty root under which there may be grammar element children.
    If there is a * or ?, or any of the sort, we now have a list child that contains many tree roots.
        - These tree roots may be single grammar element roots or empty group roots.
    A nice prune is to remove empty group parents when there is only 1 child (keeps the tree simpler).
*/
bool ParseTokensWithGrammar(
    TokenContainer &tokens, 
    const grammar_definition &grammarDef,
    struct tree_node *tree,
    ppl_error_context &errorCtx,
    bool parentWantsVerboseAST)
{
    
    if (!GRAMMAR.DefExists(grammarDef.name)) return false;

    errorCtx.currTopLevelTree = tree;

    //buffered_errors = []
    
    // NOTE(Noah): Here we do not alloc another string.
    // The string has already been secured and alloced inside of grammarDef.
    *tree = CreateTree(AST_GNODE);
    tree->metadata.str = grammarDef.name;
    
    // Fill up the tree with any parsed children.
    bool r;
    if ( SillyStringEquals(  grammarDef.name, "expression" ) ) {
        r = ParseTokensWithPrattParser( tokens, grammarDef.regexTree, grammarDef, *tree, errorCtx, parentWantsVerboseAST );
    } else {
        r = ParseTokensWithRegexTree(tokens, grammarDef.regexTree, grammarDef, *tree, errorCtx, parentWantsVerboseAST);
    }

    //if ( SillyStringStartsWith(grammarDef.name, "program") )
        //buffered_errors += (_buffered_errors);
    
    // NOTE(Noah): I know this is going to work and do what I want it to do, but 
    // I would say that there MUST be a better way to do this...
    // Namely I do not like len(trees) == 1...
    
    /*
    if ( fail_flag && len(trees) == 1 ) {
        if (GRAMMAR.DefExists(grammarDef.name)) {
            for eq in g.equivalences[grammarDef.name]:
                if eq == trees[0].data:
                    fail_flag = False # redemption.
                    return (trees[0], buffered_errors) # override upper level grammar object. Makes AST simpler.
            
        }
    }
    */

    if (!r) {
        // Delete any children that might have been created.
        for (unsigned int i = 0; i < tree->childrenCount; i++) {
            DeallocTree(tree->children[i]);
        }
        tree->childrenCount = 0; 
        return false;
    } 
    return true;
}
   

/*
def Run(tokens, logger):
    grammar = g.LoadGrammar()
    abstractSyntaxTree, bufErrors = ParseTokensWithGrammar(tokens, grammar, grammar.defs["program"], logger)
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

struct {
    token_type tokType;
    int prec; /* Highest precedence is the thing that happens first. */ 
} g_operator_precedence_table[] = {    
    { TOKEN_OP_MEMBER_SELECTION, 1 }, // Member selection
    { TOKEN_OP_INCREMENT, 1 }, // suffix/Postfix increment 
    { TOKEN_OP_DECREMENT, 1 }, // suffix/Postfix decrement 

// TODO!!!!
#if 0
    { TOKEN_OP_PAREN, 1 }, // Function call
    { TOKEN_OP_ARRAY_SUBSCRIPT, 1 }, // Array subscripting
#endif

    { TOKEN_OP_LOGICAL_NOT, 2 }, // Logical NOT 
    { TOKEN_OP_BITWISE_NOT, 2 }, // Bitwise NOT

    { TOKEN_OP_MULTIPLICATION, 3 }, // Multiplication
    { TOKEN_OP_DIVISION, 3 }, // Division
    { TOKEN_OP_MODULUS, 3 }, // Modulus

    { TOKEN_OP_ADDITION, 4 },    // Addition
    { TOKEN_OP_SUBTRACTION, 4 }, // Subtraction
      
    { TOKEN_OP_BITWISE_LEFT_SHIFT, 5 }, // Bitwise left shift
    { TOKEN_OP_BITWISE_RIGHT_SHIFT, 5 }, // Bitwise right shift

    { TOKEN_OP_LESS_THAN, 6 }, // Less than
    { TOKEN_OP_LESS_THAN_OR_EQUAL_TO, 6 }, // Less than or equal to
    { TOKEN_OP_GREATER_THAN, 6 }, // Greater than
    { TOKEN_OP_GREATER_THAN_OR_EQUAL_TO, 6 }, // Greater than or equal to

    { TOKEN_OP_EQUAL_TO, 9 }, // Equal to
    { TOKEN_OP_NOT_EQUAL_TO, 9 }, // Not equal to

    { TOKEN_OP_BITWISE_AND, 10 }, // Bitwise AND

    { TOKEN_OP_BITWISE_XOR, 11 }, // Bitwise XOR

    { TOKEN_OP_BITWISE_OR, 12 }, // Bitwise OR

    { TOKEN_OP_LOGICAL_AND, 13 }, // Logical AND

    { TOKEN_OP_LOGICAL_OR, 14 }, // Logical OR

    { TOKEN_OP_TERNARY_CONDITIONAL, 15 }, // Ternary conditional
    
    { TOKEN_OP_ASSIGNMENT, 16 }, // Assignment
    { TOKEN_OP_ADDITION_ASSIGNMENT, 16 }, // Addition assignment
    { TOKEN_OP_SUBTRACTION_ASSIGNMENT, 16 }, // Subtraction assignment
    { TOKEN_OP_MULTIPLICATION_ASSIGNMENT, 16 }, // Multiplication assignment
    { TOKEN_OP_DIVISION_ASSIGNMENT, 16 }, // Division assignment
    { TOKEN_OP_MODULUS_ASSIGNMENT, 16 }, // Modulus assignment
    { TOKEN_OP_LEFT_SHIFT_ASSIGNMENT, 16 }, // Left shift assignment
    { TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT, 16 }, // Right shift assignment
    { TOKEN_OP_BITWISE_AND_ASSIGNMENT, 16 }, // Bitwise AND assignment
    { TOKEN_OP_BITWISE_XOR_ASSIGNMENT, 16 }, // Bitwise XOR assignment
    { TOKEN_OP_BITWISE_OR_ASSIGNMENT, 16 }, // Bitwise OR assignment
    
    { TOKEN_OP_COMMA, 17 } // Comma operator
};


// TODO: maybe this gets a new interface where the compelled_op is not a string and is instead the enum
// already? in fact, maybe our token representation already stores the OP enums. that seems like the most
// sensible approach.
bool ParseOperator(TokenContainer &tokens,
    token_type                     compelled_op,
    tree_node                     *tree,
    ppl_error_context             &errorCtx,
    const grammar_definition      &currGrammarCtx)
{
    struct token tok = tokens.QueryNext();
    
    bool bCompelledOp = (compelled_op != TOKEN_UNDEFINED);
    bool didMatch;
    
    // TODO: right now it feels like we could do a function call like
    // myfunction)param1, param2) and this would be valid. looks dumb, but I don't really have
    // a problem with this right now. not that critical.
    
    switch(tok.type) {
        CASE_TOKEN_OP
        CASE_TOKEN_OP_COMPOUND
            didMatch = bCompelledOp ? compelled_op == tok.type : true;
            break;
        default:
            didMatch =false;
    }
    

    if (didMatch) {
        tokens.AdvanceNext();
        struct tree_node newTree = CreateTree( TokenToAstOp(tok.type) );

        *tree = newTree;
        
        return true;
    } 
    else {

        // TODO: get this error stuff working again.
#if 0
        if (errorCtx.SubmitError(
            PPL_ERROR_KIND_PARSER,
            tok.line, tok.beginCol, tokens.GetSavepoint()
        ))
        {
            bCompelledOp ? snprintf(errorCtx.errMsg,
                PPL_ERROR_MESSAGE_MAX_LENGTH,
                "Expected op '%.*s'. Grammar context = %s.",
                strlen(op),
                op,
                currGrammarCtx.name) : snprintf(errorCtx.errMsg,
                                                PPL_ERROR_MESSAGE_MAX_LENGTH,
                                                "Expected op. Grammar context = %s.",
                                                currGrammarCtx.name)
            ;

            g_bufferToPrintTo = errorCtx.almostParsedTree;
            g_bufferToPrintToLen = PPL_ERROR_PARTIAL_AST_MAX_LENGTH;
            g_lastBufPos         = 0;

            PrintTree(*errorCtx.currTopLevelTree, 0, HackyPrintToBuffer);
        }
#endif

        return false;
    }
}

bool ParseLiteral(    TokenContainer &tokens,       tree_node *tree,  ppl_error_context &errorCtx )
{
    struct token tok = tokens.QueryNext(); // # the whole LR k+1 idea :)
    bool didMatch = true;

    // TODO: it would be nice if these switch statements could also warn us at compile-time
    // that we are missing a case. Is there an elegant way in C++ to do this or is this one
    // of those things that would be best done in the new language?

    struct tree_node newTree;
    switch (tok.type) {

        case TOKEN_QUOTE: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_STRING_LITERAL);
            newTree.metadata.str     = tok.str;
        } break;

        // 
        case TOKEN_NULL_LITERAL: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_NULL_LITERAL);
        } break;

        case TOKEN_TRUE_LITERAL: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_INT_LITERAL, true);
        } break;

        case TOKEN_FALSE_LITERAL: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_INT_LITERAL, false);
        } break;

        case TOKEN_DOUBLE_LITERAL: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_DECIMAL_LITERAL, (double)tok.dnum);
        } break;
        case TOKEN_FLOAT_LITERAL: {
            // TODO: I know that f64 is more bits than f32, but is there e.g. a loss in precision
            // when we cast from double to float? some odd floating point representation thing.
            tokens.AdvanceNext();
            newTree = CreateTree(AST_DECIMAL_LITERAL, (float)tok.dnum);
        } break;

        case TOKEN_UINT_LITERAL:
        {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_INT_LITERAL, tok.num);
        } break;

        case TOKEN_INTEGER_LITERAL: {
            tokens.AdvanceNext();
            
            // TODO: What happens if we write into the program a value that is too large
            // to fit into int64_t ?
            // or in general whenever we are looking at type stuff.
            // -> this should  be a compiler error about a non-explicit truncation.
            assert( tok.num <= uint64_t(INT64_MAX) );

            newTree = CreateTree(AST_INT_LITERAL, (int64_t)tok.num);
        } break;
        case TOKEN_CHARACTER_LITERAL: {
            tokens.AdvanceNext();

            // TODO: revisit this stuff.
            assert( tok.c <= uint64_t(INT8_MAX) );

            newTree = CreateTree(AST_INT_LITERAL, (char)tok.c);
        } break;
        // TODO: add the rest. can do so with a table idea.
        case TOKEN_KEYWORD_BOOL: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_TYPE_LITERAL, PPL_TYPE_BOOL);
        } break;
        case TOKEN_KEYWORD_INT: {
            tokens.AdvanceNext();
            newTree = CreateTree(AST_TYPE_LITERAL, PPL_TYPE_S64);
        } break;
        default:
            didMatch = false;
            break;
    }

    if (!didMatch) {

        // emit error!
        if (errorCtx.SubmitError(
            PPL_ERROR_KIND_PARSER,
            tok.line, tok.beginCol, tokens.GetSavepoint()
        ))
        {
            snprintf(errorCtx.errMsg, PPL_ERROR_MESSAGE_MAX_LENGTH,
            "Expected a literal but sure as hell did not get one.");
        }

        // NOTE: so the savepoint idea gets the index of the token that
        // is returned by QueryNext. so the savepoint here is correctly
        // the token that we just failed on.

        return false;
    }
    
    *tree = newTree;

    // re_matched += 1;
    return true;
}

bool ParseSymbol(TokenContainer &tokens, tree_node *tree, ppl_error_context &errorCtx)
{
    struct token tok = tokens.QueryNext();  //# the whole LR k+1 idea :)
    if (tok.type == TOKEN_SYMBOL) {
        tokens.AdvanceNext();
        struct tree_node newTree = CreateTree(AST_SYMBOL);
        newTree.metadata.str     = tok.str;
        *tree                    = newTree;
        return true;
    } else {
        if (errorCtx.SubmitError(PPL_ERROR_KIND_PARSER, tok.line, tok.beginCol, tokens.GetSavepoint())) {
            snprintf(errorCtx.errMsg, PPL_ERROR_MESSAGE_MAX_LENGTH, "Expected a symbol.");
        }

        return false;
    }
}

bool ParseTypeLiteral(TokenContainer &tokens, tree_node *tree, ppl_error_context &errorCtx) {
    // TODO: this grammar object is quite broad.
}

// TODO: okay, it's time to watch the stream again. it feels like things are more complicated than they need to be.

/*

RIGHT LEANING

1 > 2 + 3 * 4
1 > (2 + (3 * 4))

    >
   / \
  1   +
     / \
    2   *
       / \
      3   4

parse_leaf():
    get the leaf thing/

parse_single_binary():
    left = parse_leaf()
    operator = parse_op()
    right = parse_leaf()
    return make_binary()

parse_expression():
    left = parse_leaf()

    next = op()

    if is_binary_op(next) {
        right = parse_leaf()
        right make_binary()
    } else {
        return left
    }

make it recursive,

parse_expression():
    left = parse_leaf()
    next = op()
    if !is_binary_op(next) return left

    right = parse_expression()
    return make_binary()

now, the above code will generate a right leaning tree.
how could we create a left leaning tree?

LEFT LEANING

1 * 2 + 3 - 4
((1 * 2) + 3) - 4)

      -
     / \
    +   4
   / \
  *   3
 / \
1   2

parse_binary(left):
    op = parse_op()
    if !is_binary_op(next) return left

    right = parse_left()
    return make_binary(left, op, right)

parse_expression():
    left = parse_leaf()
    
    while(true) {
        new_tree = parse_binary(left)

        if new_tree == left return left

        left = new_tree
    }

the key idea (insight) to pratt parser is that ... 

right leaning tree (or right associative tree).
correct when ... precedence is increasing.

left leaning tree (or left associative tree).
correct when ... precedence is decreasing.

what we can do is to build a left leaning tree where some of the rhs are right leaning trees.

here is the new code:

parse_increasing_precedence(left, min_prec):
    next = get_next_token()
    if !is_binary_op(next) return left

    next_prec = get_prec(next)

    if next_prec <= min_prec {
        right = parse_left()
        return make_binary(left, op, right)
    } else {
        right = parse_expression(next_prec)
        return make_binary(left, op, right)
    }

parse_expression(min_prec):
    left = parse_leaf()

    while true {
        new_tree = parse_increasing_precedence(left, min_prec)
        if new_tree == left return left
        left = new_tree
    }
*/

bool IsTokBinaryOp(token tok)
{
    bool bIsOp = true;
    bool bIsUnary = false;
    switch(tok.type) {
        CASE_TOKEN_BINARY_OP
            break;
        CASE_TOKEN_UNARY_OP
            bIsUnary = true;
            break;
        // NOTE: in the above switch, close paren is consider not unary. while close paren is not
        // unary, it is also not a binary op. so for the sake of the code here, we'll allow it to be
        // "unary".
        default:
            bIsOp = false;
    }

    return (bIsOp && !bIsUnary);
}

#if 0
// returns the token count parsed as an expression.
// if we get back zero, it means the sequence of tokens currently pointed at
// does not begin with an expression at all.
int ParseTokensAsRightLeaningTree(
    TokenContainer &tokens,
    tree_node *tree,
    ppl_error_context &errorCtx,
    const grammar_definition &currGrammarCtx
)
{
    // lhs = leaf.
    // rhs = tree.
    int tokenCount = 0;
    tree_node literal;
    tree_node op;
    // NOTE: a literal is 1 token.
    // NOTE: an operator is 1 token.
    bool r = ParseLiteral( tokens, &literal, errorCtx ) || ParseSymbol( tokens, &literal, errorCtx );
    if (!r){
        // it wasn't a literal, is it a unary?
        struct token tok = tokens.QueryNext(); // # the whole LR k+1 idea :)
        bool bIsUnary = false;
        switch(tok.type) {
            CASE_TOKEN_UNARY_OP
            bIsUnary = true;
            if (tok.c == ')') bIsUnary = false; // only open paren is unary.
        }
        if (bIsUnary) {
            if (ParseOperator( tokens, TOKEN_UNDEFINED, &op, errorCtx, currGrammarCtx ) ) {
                tokenCount += 1;

                tree_node rlt;
                tokenCount += ParseTokensAsRightLeaningTree(tokens, &rlt, errorCtx, currGrammarCtx);
                if (tokenCount > 1) {

                    // handle special syntax.
                    if ( tok.type == TOKEN_PART && tok.c == '(' ) {
                        struct token closeParen = tokens.QueryNext(); 
                        if ( closeParen.type == TOKEN_PART && closeParen.c == ')' ) {
                            tokens.AdvanceNext();
                            tokenCount += 1;
                        }

                    }

                    TreeAdoptTree(op, rlt);
                    *tree = op;

                }
            } else {
                PPL_TODO; // if it's a unary op, parsing the operator should not fail.
            }
        }
    } else {
        tokenCount += 1;

        // expect a binary operator.
        struct token tok = tokens.QueryNext(); // # the whole LR k+1 idea :)
        bool bIsOp = true;
        bool bIsUnary = false;
        switch(tok.type) {
            CASE_TOKEN_BINARY_OP
                break;
            CASE_TOKEN_UNARY_OP
                bIsUnary = true;
                break;
            // NOTE: in the above switch, close paren is consider not unary. while close paren is not
            // unary, it is also not a binary op. so for the sake of the code here, we'll allow it to be
            // "unary".
            default:
                bIsOp = false;
        }

        if (bIsOp && !bIsUnary) {
            if (
                // passing NULL means it could be any operator.
                ParseOperator( tokens, TOKEN_UNDEFINED, &op, errorCtx, currGrammarCtx )
            ) {
                tokenCount += 1;
                tree_node rlt;
                tokenCount += ParseTokensAsRightLeaningTree(tokens, &rlt, errorCtx, currGrammarCtx);
                if (tokenCount > 2) {
                    TreeAdoptTree(op, literal);
                    TreeAdoptTree(op, rlt);
                    *tree = op;
                }
            } else {
                // shouldn't fail here. this is a bug.
                PPL_TODO;
            }            
        } else {
            // this is the terminal case, since we got a literal, but no operator. that's a valid
            // expression.
            *tree = literal;
        }        
    }
    
    return tokenCount;
}
#endif

bool ParseExpressionLeaf(TokenContainer &tokens, tree_node *tree, ppl_error_context &errorCtx)
{
    return ParseLiteral( tokens, tree, errorCtx ) || ParseSymbol( tokens, tree, errorCtx );
}

// make the tree about op.
tree_node MakeBinary(const tree_node &left, const tree_node &op, const tree_node &right)
{
    tree_node newOp = op;
    TreeAdoptTree(newOp, left);
    TreeAdoptTree(newOp, right);
    return newOp;
}

// TODO: this could be even more efficient if we use the enum as an index.
int OpPrecedence(const token &op)
{
    size_t opsCount = ARRAY_SIZE(g_operator_precedence_table);
    for (int i=0; i < opsCount;i++)
    {
        auto thing = g_operator_precedence_table[i];
        if (thing.tokType== op.type){
            return thing.prec; 
        }
    }
    PPL_TODO; // this is bug
    return 0;
}

tree_node ParseExpression(TokenContainer &tokens, int prec, ppl_error_context &errorCtx,     const grammar_definition &currGrammarCtx);

// TODO: think about unary after.
// TODO: think about errors after.
// TODO: why we pass around so many contexts :(

bool ParseIncreasingPrecedence(TokenContainer &tokens, const tree_node &left, int prec, tree_node *tree, 
ppl_error_context &errorCtx,     const grammar_definition &currGrammarCtx)
{
    struct token tok = tokens.QueryNext(); // # the whole LR k+1 idea :)

    tree_node op;
    tree_node right;

    if (!IsTokBinaryOp(tok)) {
        *tree = left;
        return true; // terminal condition.
    }

    int next_prec = OpPrecedence(tok);

    if (next_prec > prec) {
#if 0
        bool r = ParseExpressionLeaf(tokens, &right, errorCtx);
        auto tn = MakeBinary(left, op, right);
        *tree = tn;
#else
        *tree = left;
        return true;
#endif
    } else {
        ParseOperator(tokens, TOKEN_UNDEFINED, &op, errorCtx, currGrammarCtx);
        right = ParseExpression(tokens, next_prec, errorCtx, currGrammarCtx);
        auto tn = MakeBinary(left, op, right);
        *tree = tn;
    }

    return false;
}

// prec is precedence.
tree_node ParseExpression(TokenContainer &tokens, int prec, ppl_error_context &errorCtx,     const grammar_definition &currGrammarCtx)
{
    tree_node left;
    ParseExpressionLeaf(tokens, &left, errorCtx);

    while (true) {
        tree_node new_tree;
        bool bSame = ParseIncreasingPrecedence(tokens, left, prec, &new_tree, errorCtx, currGrammarCtx);
        if (bSame) return left;
        left = new_tree;
    }
}

bool ParseTokensWithPrattParser(
    TokenContainer &tokens, 
    const tree_node &regexTree,
    const grammar_definition &currGrammarCtx,
    tree_node &tree,
    ppl_error_context &errorCtx,
    bool parentWantsVerboseAST)
{


    
    // TODO: interweve them. for now we parse as a right leaning tree.

    tree_node exp = ParseExpression(
        tokens, 1000,
        errorCtx, currGrammarCtx
    );
    tree = exp;
    // TODO: handle failure later. maybe we can have the error be indicated in the error context.

#if 0
    // assert( (tokenCount >= 3) && (tokenCount % 2 != 0) );
    if ( tokenCount >= 2 ) {
        TreeAdoptTree(tree, exp);
    }
    return tokenCount >= 2;
#endif

    return true;
}
