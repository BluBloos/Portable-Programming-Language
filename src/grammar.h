#ifndef GRAMMAR_H
#define GRAMMAR_H
#include <tree.h>

// TODO(Noah): In making the AST tree dramatically simpler, 
//      create ghost grammar definitions.
// these definitions will not create a corresponding gnode and will
// instead just "passthrough" the children to the request node. 

/*
equivalences = {}
equivalences["term"] = ["factor", "object"]
equivalences["additive_exp"] = ["term", "factor", "object"]
equivalences["relational_exp"] = ["additive_exp", "term", "factor", "object"]
equivalences["equality_exp"] = ["relational_exp", "additive_exp", "term", "factor", "object"]

equivalences["bitwise_and_exp"] = ["equality_exp", "relational_exp", "additive_exp", "term", "factor", "object"]
equivalences["bitwise_or_exp"] = ["bitwise_and_exp","equality_exp", "relational_exp", "additive_exp", "term", "factor", "object"]    

equivalences["logical_and_exp"] = ["bitwise_or_exp","bitwise_and_exp","equality_exp", "relational_exp", "additive_exp", "term", "factor", "object"] 
equivalences["logical_or_exp"] = ["logical_and_exp", "bitwise_or_exp","bitwise_and_exp","equality_exp", "relational_exp", "additive_exp", "term", "factor", "object"]
equivalences["conditional_exp"] = ["logical_or_exp", "logical_and_exp", "bitwise_or_exp","bitwise_and_exp","equality_exp", "relational_exp", "additive_exp", "term", "factor", "object"]

def IsValidType(typeStr):
    return typeStr in lexer.TYPES
*/


// NOTE(Noah): Grammar definition are custom regular expressions that I invented,
// regardless of if there are parsing libraries out there...
struct grammar_definition {
    const char *name;
    struct tree_node regexTree;
};

struct Grammar {
    bool bIsLoaded = false;
    Grammar() {}
    ~Grammar() {
        // Go through all definitions in defs and dealloc the trees.
        for (auto kv : defs) {
            struct grammar_definition &gd = kv.second;
            DeallocTree(gd.regexTree);
        }
    }
    std::unordered_map<std::string, struct grammar_definition> defs;
    bool DefExists(const char *defName) {
        return defs.count(defName);
    }
    // NOTE(Noah): AddDef doesn't check to ensure that the defName you supplied already exists.
    // It assumes this responsibility to the caller.
    void AddDef(const char *defName, struct grammar_definition gd) {
        defs[defName] = gd;
    }
    void AddDef(const char *defName) {
        struct grammar_definition gd = {};
        defs[defName] = gd;
    }
    // NOTE(Noah): Does no checking.
    struct grammar_definition GetDef(const char *defName) {
        return defs[defName];
    }
    void Print() {
        for (auto kv : defs) {
            struct grammar_definition &gd = kv.second;
            struct tree_node &tn = gd.regexTree;
            LOGGER.Log("Grammar definition for %s:", gd.name);
            PrintTree(tn, 0);
        }
    }
};

Grammar GRAMMAR = Grammar(); // global grammar object.

// TODO(Noah): Account for failure to generate RegexTree. Do something sensible.
struct tree_node CreateRegexTree(Grammar &grammar, const char *regex) {
    
    struct tree_node regexTree = CreateTree(TREE_ROOT);
    struct tree_node *contextStack = NULL; // stretch buffer.
    StretchyBufferInit(contextStack);
    StretchyBufferPush(contextStack, regexTree);

    int regexLen = strlen(regex);
    int n = 0;

    while (n < regexLen) {
        char c = regex[n]; 
        struct tree_node &lastTree = StretchyBufferLast(contextStack);
        // Enter in and out of context stacks.
        if (c == '[') {
            StretchyBufferPush(contextStack, CreateTree(TREE_REGEX_ANY));
        }
        else if (c == '(') {
            // Search until ')'
            std::string newStr = "";
            for (int j = n + 1; j < regexLen; j++) {
                if (regex[j] != ')') {
                    newStr += regex[j];
                } else {
                    break; // or something like this.
                }
            } 
            if (grammar.DefExists(newStr.c_str()) || newStr == "symbol" || newStr == "keyword" || newStr == "literal") {
                struct tree_node tn = CreateTree(TREE_REGEX_STR, newStr.c_str());
                if (newStr == "symbol"  ) tn.type = TREE_REGEX_SYMBOL;
                if (newStr == "keyword"  ) tn.type = TREE_REGEX_GENERIC_KEYWORD;
                if (newStr == "literal"  ) tn.type = TREE_REGEX_LITERAL;
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else if (SillyStringStartsWith(newStr.c_str(), "keyword=")) {
                struct tree_node tn = CreateTree(TREE_REGEX_KEYWORD, (newStr.c_str()+strlen("keyword=")) );
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else if (SillyStringStartsWith(newStr.c_str(), "op,")) {
                struct tree_node tn = CreateTree(TREE_REGEX_OP, SillyStringFmt("op%s", newStr.c_str()+strlen("op,")));
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else {
                StretchyBufferPush(contextStack, CreateTree(TREE_REGEX_GROUP));
            }
        } 
        else if (c == ']' || c == ')') {
            StretchyBufferPop(contextStack);
            TreeAdoptTree(StretchyBufferLast(contextStack), lastTree);
        }
        else if (c == '?' || c == '*' || c == '+' || c == '`') {
            // NOTE(Noah): This if statement actually never evaluates to true it seems...
            // We can tree the Any/Group simply as children of a tree.
            if (lastTree.childrenCount == 0) {
                // if there are no children, then for sure the last thing added to regex tree
                // was in fact the Any/Group root node itself.  
                lastTree.metadata.regex_mod = c;
            } else {
                // preceeding_re is assumed to be a child of a tree
                lastTree.children[lastTree.childrenCount - 1].metadata.regex_mod = c;    
            }
        } 
        else if (c == '\\') { 
            // the escape sequence:
            // TODO(Noah): Investigage if this is unsafe or something like this...
            TreeAdoptTree(lastTree, CreateTree(TREE_REGEX_CHAR, regex[n+1]));
            n += 1; // make sure to skip over the character that we escaped so we do not double it
        } 
        else { 
            // just a character that we are trying to match
            TreeAdoptTree(lastTree, CreateTree(TREE_REGEX_CHAR, c));
        }
        n += 1;
    }
    struct tree_node r = contextStack[0];
    StretchyBufferFree(contextStack);
    return r;
}

struct grammar_definition CreateGrammarDefinition(const char *name, const char *regExp) {
    struct grammar_definition gd;
    gd.name = name; 
    gd.regexTree = CreateRegexTree(GRAMMAR, regExp);
    return gd;
}

struct grammar_definition CreateGrammarDefinition(
    const char *name, const char *beta, const char *alpha
) {
    // here we are defining a left recursive grammar.
    struct grammar_definition gd;
    gd.name = name;
    char * regExp = MEMORY_ARENA.StringAlloc(SillyStringFmt("%s%s*", beta, alpha));
    gd.regexTree = CreateRegexTree(GRAMMAR, regExp);
    return gd;
}

/*
So basically, the issue here is that for each Grammar.AddDef call, we construct the RegexTree.
but in doing so, we require to know what type of grammar definitions already exist.
*/

// TODO(Noah): Look into making these grammar constructions more readable.
// I think the idea of string to regex tree is silly. we should maybe just directly construct the regex tree.

char *_grammarTable[][2] =
{
    {   
        "program",
        "((compile_time_var_decl);?)*"
    },
    {
        "route",
        "(symbol):"
    },
    { 
        // TODO: this doesn't keep what keyword was matched. there is no node generated for this.
        // so we are losing information.
        "qualifier",
        "[(keyword=static)(keyword=unsigned)]`"
    },
    {
        "pointer_type",
        "(op,^)(type)"
    },
    {
        // TODO: Change to integer_literal.
        "array_type",        
        "(op,[)(literal)?(op,])(type)"
    },

    {
        "function_type",
        "(keyword=fn)"
        "\\("
            "("
                "(runtime_var_decl)(,(runtime_var_decl))*"
            ")?"
        "\\)"
        "((op,->)(type))?"
    },

    // NOTE: types are merely an expression of kind Type. e.g. if we do a #run on a function that
    // returns a Type, this is valid, and could be used with variable declarations.
    //
    // we're going to want to parse these as full expressions. Even if Type is not defined for things like `+`,
    // we could emit an error on the SEMA side saying -> "Type not defined for `+`"".    
    {
        "type_literal",
        "[(function_type)(pointer_type)(array_type)(type_keyword)((qualifier)(type))]"
    },

    // TODO: currently this grammar definition is just a proxy.
    {
        "type",
        "(conditional_exp)"
    },

    // TODO: we could make type_keyword on the lexing side be their own tokens and give them
    // type of PPL_TYPE_TYPE. maybe call this (type_literal) and change type_literal -> type_construction.
    //
    // I also see value in this kind of *_literal idea.
    // like e.g. I might want an integer_literal one, or a string_literal.
    {
        "type_keyword",
        // TODO: is there anyway to make the below less crap?
        // especially because it has terrible performance implications.
        "[(keyword=float)(keyword=double)(keyword=f32)(keyword=f64)(keyword=bool)(keyword=void)(keyword=u8)(keyword=u16)(keyword=u32)(keyword=u64)(keyword=s8)(keyword=s16)(keyword=s32)(keyword=s64)(keyword=int)(keyword=char)(keyword=short)(keyword=Any)(keyword=Type)(keyword=TypeInfo)(keyword=TypeInfoMember)(keyword=struct)(keyword=enum)(keyword=enum_flag)(keyword=namespace)]`"
    },

    {
        "data_pack",
        "(type)?\\{(statement)*[(statement)(statement_noend)]?\\}"
    },

    {
        "untyped_data_pack",
        "\\{(statement)*[(statement)(statement_noend)]?\\}"
    },

    {
        "return_statement",
        "(keyword=return)(expression)"
    },
    
    // TODO: maybe a var decl should always have the semicolon baked in?
    {
        "runtime_var_decl",
        "(route)"
        "["
            "("
                "(type)"
                "("
                    "(op,=)"
                    "[(op,?)(expression)]"
                ")?"
            ")"
            "((op,=)(expression))"
        "]"
    },
    {
        "compile_time_var_decl",
        "(route):"

        // optional modification to make this compile-time var a template.
        "("
            "(op,<)"
            "["
                "((route)(type))"
                "(symbol)"                                               
            "]"
            "["
                "((op,,)(route)(type))"
                "((op,,)(symbol))"                
            "]*"
            "(op,>)"
        ")?"

        "[((type)(op,=)(expression))(expression)]"
    },
    {
        "var_decl",
        "[(compile_time_var_decl)(runtime_var_decl)]"
    },
    {
        // includes the else statement :p
        "if_statement",
        "(keyword=if)(expression)[;(keyword=then)](statement)((keyword=else)(statement))?"
    },

    {
        "while_statement",
        "(keyword=while)"
        "["
            "((statement)(expression);(statement_noend))"
            "((expression);(statement_noend))"
            "(expression)"
        "]"
        "[(keyword=do);]"
        "(statement)"
    },

    {
        // whether the expression is set-like is another story. that is the semantic analysis stage.
        "for_statement",
        "(keyword=for)((symbol)(,(symbol))*(keyword=in))?(expression)[;(keyword=do)](statement)"
    },


    {
        // TODO: In SEMA stage verify that there is just one default case label in the switch body.
        //
        // TODO: support duffs device.
        "switch_statement",
        "(keyword=switch)(expression);"
        "\\{"
            "("
                "["
                    "((keyword=case)`(expression):)"
                    "((keyword=default)`:)"
                "]"
                "(statement)*"
            ")*"
        "\\}"
    },


    {
        "statement",
        "[((compile_time_var_decl);?)(untyped_data_pack);([(return_statement)(runtime_var_decl)(expression)(keyword=fall)`(keyword=break)`(keyword=continue)`];)(if_statement)(while_statement)(for_statement)(switch_statement)]"
    },
    { 
        // TODO: this grammar is sort of not correct. e.g. if_statement ends with a `;`, so how can we say that this is a statement,
        // with on end?
        "statement_noend",
        "[(untyped_data_pack)(var_decl)(expression)(return_statement)(keyword=fall)`(keyword=break)`(keyword=continue)`(if_statement)(while_statement)(for_statement)(switch_statement)]"
    },
    {
        "span_expression",
        // TODO: change the below to integer_literal.
        "(literal)[(op,..=)(op,..<)](literal)"
    },
    {
        "assignment_exp",
        "(factor)[(op,=)(op,+=)(op,<<=)(op,>>=)(op,^=)(op,-=)(op,*=)(op,/=)(op,%=)(op,&=)(op,|=)](expression)"
    },
    {
        "conditional_exp",
        "(logical_or_exp)(\\?(expression):(expression))?"
    },
    {
        "logical_or_exp",
        "(logical_and_exp)((op,||)(logical_and_exp))*"
    },
    {
        "logical_and_exp",
        "(bitwise_or_exp)((op,&&)(bitwise_or_exp))*"
    },
    {
        "bitwise_or_exp",
        "(bitwise_xor_exp)((op,|)(bitwise_xor_exp))*"
    },
    {
        "bitwise_xor_exp",
        "(bitwise_and_exp)((op,^)(bitwise_and_exp))*"
    },
    {
        "bitwise_and_exp",
        "(equality_exp)((op,&)(equality_exp))*"
    },
    {
        "equality_exp",
        "(relational_exp)([(op,==)(op,!=)](relational_exp))*"
    },
    {
        "relational_exp",
        "(bitshift_exp)([(op,>=)(op,<=)(op,<)(op,>)](bitshift_exp))*"
    },
    {
        "bitshift_exp",
        "(additive_exp)([(op,<<)(op,>>)](additive_exp))*"
    },
    {
        "additive_exp",
        "(term)([(op,+)(op,-)](term))*"
    },
    {
        // TODO: needs a better name.
        "term",
        "(factor)([(op,*)(op,/)(op,%)](factor))*"
    },
    {
        // TODO: needs a better name.
        // TODO: okay so based on the way that I lay out these strings below,
        // it almost appears like I could do some C++ magic to construct a meta-system
        // for construction of these regex trees :)
        "factor",
        "["
            "((op,.)(object))"
            "(object)"
            // NOTE: for now, I think the C-style cast grammar is OK.
            // since "(\\((expression)\\))" is a subset of "((\\((type)\\)(factor))".
            // and otherwise we will not see "((expression)(factor))" ever.
            "([(op,++)(op,--)(op,!)(op,-)(op,*)(op,@)(op,~)(\\((type)\\))](factor))"
            "(\\((expression)\\))"
        "]"
    },

    {
        "builtin_func",
        "[(keyword=size_of)(keyword=type_of)(keyword=type_info)(keyword=offset_of)]`"
    },

    {
        "function_call",
        // NOTE: keyword is used here for builtin functions.
        "[(symbol)(builtin_func)]\\((expression)?\\)"
    },

    {
        "import_expression",
        // TODO: change the below to string_literal.
        "(keyword=#import)(literal)"
    }

};

char  *_grammarTable_LR[][3] = {
    // NOTE: these grammar constructions works by appending beta and alpha and give the alpha the `*` modifier.
    {
        "object",
        "[(function_call)(symbol)(literal)(type_literal)]", // beta
        "[(op,++)(op,--)((op,[)(expression)(op,]))((op,.)(object))]" // alpha
    },
    {
        "expression",
        "[(import_expression)(data_pack)(span_expression)(assignment_exp)(conditional_exp)]",
        "((op,,)(expression))"
    }
};

// Loads the grammar into the global grammar object.
void LoadGrammar() {

    if (GRAMMAR.bIsLoaded) return;

    unsigned int grammarCount = sizeof(_grammarTable) / (2 * sizeof(char *));
    unsigned int grammarCountLR = sizeof(_grammarTable_LR) / (3 * sizeof(char *));

    // lube the tree
    // NOTE(Noah): ensures that when creating regex trees, the grammar is "well defined".
    {
        for ( unsigned int i = 0; i < grammarCount; i++) {
            GRAMMAR.AddDef(_grammarTable[i][0]);
        }

        for (unsigned int i = 0; i < grammarCountLR; i++) {
            GRAMMAR.AddDef(_grammarTable_LR[i][0]);
        }
    }

    // define grammars with Regex Trees pre-generated.
    {
        for (unsigned int i = 0; i < grammarCount; i++) {
            GRAMMAR.AddDef(_grammarTable[i][0], CreateGrammarDefinition(
                _grammarTable[i][0],
                _grammarTable[i][1]
            ));
        }
        
        // left-recursive grammars
        for (unsigned int i = 0; i < grammarCountLR; i++) {
            GRAMMAR.AddDef(_grammarTable_LR[i][0], CreateGrammarDefinition(
                _grammarTable_LR[i][0],
                _grammarTable_LR[i][1],
                _grammarTable_LR[i][2]
            ));
        }
    }
 
    if (VERBOSE) {
        GRAMMAR.Print();
    }

    GRAMMAR.bIsLoaded = true;
}   
#endif