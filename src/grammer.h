#ifndef GRAMMER_H
#define GRAMMER_H
#include <tree.h>

// TODO(Noah): In making the AST tree dramatically simpler, 
//      create ghost grammer definitions.
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


// NOTE(Noah): Grammer definition are custom regular expressions that I invented,
// regardless of if there are parsing libraries out there...
struct grammer_definition {
    const char *name; 
    struct tree_node regexTree;
};

class Grammer {
    public:
    Grammer() {}
    ~Grammer() {
        // Go through all definitions in defs and dealloc the trees.
        for (auto kv : defs) {
            struct grammer_definition &gd = kv.second;
            DeallocTree(gd.regexTree);
        }
    }
    std::unordered_map<std::string, struct grammer_definition> defs;
    bool DefExists(const char *defName) {
        return defs.count(defName);
    }
    // NOTE(Noah): AddDef doesn't check to ensure that the defName you supplied already exists.
    // It assumes this responsibility to the caller.
    void AddDef(const char *defName, struct grammer_definition gd) {
        defs[defName] = gd;
    }
    void AddDef(const char *defName) {
        struct grammer_definition gd = {};
        defs[defName] = gd;
    }
    // NOTE(Noah): Does no checking.
    struct grammer_definition GetDef(const char *defName) {
        return defs[defName];
    }
    void Print() {
        for (auto kv : defs) {
            struct grammer_definition &gd = kv.second;
            struct tree_node &tn = gd.regexTree;
            LOGGER.Log("Grammer definition for %s:", gd.name);
            PrintTree(tn, 0);
        }
    }
};

Grammer GRAMMER = Grammer(); // global grammer object.

// TODO(Noah): Account for failure to generate RegexTree. Do something sensible.
struct tree_node CreateRegexTree(Grammer &grammer, const char *regex) {
    
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
            if (grammer.DefExists(newStr.c_str()) || newStr == "symbol" || newStr == "keyword" || newStr == "literal") {
                struct tree_node tn = CreateTree(TREE_REGEX_STR, newStr.c_str());
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else if (SillyStringStartsWith(newStr.c_str(), "keyword=")) {
                struct tree_node tn = CreateTree(TREE_REGEX_KEYWORD, (newStr.c_str()+8) );
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else if (SillyStringStartsWith(newStr.c_str(), "op,")) {
                struct tree_node tn = CreateTree(TREE_REGEX_STR, SillyStringFmt("op%s", newStr.c_str()+3));
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
        else if (c == '?' || c == '*' || c == '+') {
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

struct grammer_definition CreateGrammerDefinition(const char *name, const char *regExp) {
    struct grammer_definition gd;
    gd.name = name; 
    gd.regexTree = CreateRegexTree(GRAMMER, regExp);
    return gd;
}

struct grammer_definition CreateGrammerDefinition(
    const char *name, const char *beta, const char *alpha
) {
    // here we are defining a left recursive grammer.
    struct grammer_definition gd;
    gd.name = name;
    char * regExp = MEMORY_ARENA.StringAlloc(SillyStringFmt("%s%s*", beta, alpha));
    gd.regexTree = CreateRegexTree(GRAMMER, regExp);
    return gd;
}

/*
So basically, the issue here is that for each Grammer.AddDef call, we construct the RegexTree.
but in doing so, we require to know what type of grammer definitions already exist.
*/

// TODO(Noah): Look into making these grammer constructions more readable.
// I think the idea of string to regex tree is silly. we should maybe just directly construct the regex tree.

char *_grammerTable[][2] =
{
    {   
        "program",
        "[(function)((var_decl);)]*"
    },
    {
        "routed_value",
        "(symbol):"
    },
    // TODO: do such built_in nodes actually contain what keyword was matched??
    {
        "built_in",
        "[(keyword=size_of)(keyword=type_of)(keyword=type_info)(keyword=offset_of)]"
    },
    { 
        "qualifier",
        "[(keyword=static)(keyword=unsigned)]"
    },
    {
        "pointer_type",
        "(op,^)(type)"
    },
    {
        // TODO: And can we just do `(literal)?`   ?
        // TODO: Can we have an integer_literal    ?
        "array_type",        
        "(op,[)[(literal)]?(op,])(type)"
    },
    {
        // TODO: need to add generics.
        "type",
        "[(pointer_type)(array_type)((qualifier)(type))(symbol)(keyword)]"
    },
    {
        "data_pack",
        "\\{(statement)*\\}"
    },

    {
        "typed_data_pack",
        "(type)(data_pack)"
    },

    {
        "return_statement",
        "(keyword=return)(expression)"
    },
    {
        "runtime_var_decl",
        "(routed_value)[((type)(op,=)(expression))((op,=)(expression))]"
    },
    {
        "compile_time_var_decl",
        "(routed_value):[((type)(op,=)(expression))(expression)]"
    },
    {
        "var_decl",
        "[(compile_time_var_decl)(runtime_var_decl)]"
    },
    {
        // includes the else statement :p
        "if_statement",
        "(keyword=if)(expression)[((keyword=then)(statement))(data_pack)]((keyword=else)(statement))?"
    },
    {
        "while_statement",
        "(keyword=while)[((statement)(expression);(statement_noend))((expression);(statement_noend))(expression)][((keyword=do)(statement))(data_pack)]"
    },

    {
        // whether the expression is set-like is another story. that is the semantic analysis stage.
        "for_statement",
        "(keyword=for)((symbol)(,(symbol))*(keyword=in))?(expression)[((keyword=do)(statement))(data_pack)]"
    },


    {
        // TODO: support duffs device.
        "switch_statement",
        "(keyword=switch)(expression)\\{((keyword=case)(expression):(statement)*)*((keyword=default):(statement)*)?\\}"
    },


    {
        "statement",
        "[;([(return_statement)(var_decl)(expression)(keyword=fall)(keyword=break)(keyword=continue)];)(if_statement)(while_statement)(for_statement)(switch_statement)]"
    },
    { 
        // TODO(Noah): I would certainly like to remove this grammer definition. there is a lot that is similar between this one and the grammar for `statement`.
        "statement_noend",
        "[(var_decl)(expression)(return_statement)(keyword=fall)(keyword=break)(keyword=continue)(if_statement)(while_statement)(for_statement)(switch_statement)]"
    },


    {
        "expression",
        "[(assignment_exp)(conditional_exp)]"
    },
    {
        "assignment_exp",
        "(factor)[(op,=)(op,+=)(op,-=)(op,*=)(op,/=)(op,%=)(op,&=)(op,|=)](expression)"
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
        "(bitwise_and_exp)((op,|)(bitwise_and_exp))*"
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
        "(additive_exp)([(op,>=)(op,<=)(op,<)(op,>)](additive_exp))*"
    },
    {
        "additive_exp",
        "(term)([(op,+)(op,-)](term))*"
    },

    {
        // TODO: once we get the comma operator working, we can simplify some of the grammar below.
        "function_call",
        "[(symbol)(built_in)]\\(((expression)(,(expression))*)?\\)"
    },

    {
        // TODO: needs a better name.
        "term",
        "(factor)([(op,*)(op,/)(op,%)](factor))*"
    },
    {
        // TODO: needs a better name.
        "factor",
        "[(object)([(op,!)(op,-)(op,&)(op,*)(op,~)(\\((type)\\))](factor))(\\((expression)\\))]"
    },

    /*  
    {
        "lv", // left-value.
        "(routed_value)(type)"
    },
    */

    /*
    {
        "function",
        "(symbol):\\(((lv)(,(lv))*)?\\)((op,->)(type))?[;(statement)]"
    },
    */

    /*
    {
        // TODO: we can use data packs here. will allow e.g. member functions.
        "struct_decl",
        "(symbol):(keyword=struct)\\{((var_decl);)*\\};"
    },
    */

};

char  *_grammerTable_LR[][3] = {
    {
        "object",
        "[(function_call)(symbol)(literal)(typed_data_pack)(type)]", // beta
        "[(op,++)(op,--)((op,[)(expression)(op,]))((op,.)(object))]" // alpha
    }
};

// Loads the grammer into the global grammer object.
void LoadGrammer() {

    unsigned int grammerCount = sizeof(_grammerTable) / (2 * sizeof(char *));
    unsigned int grammerCountLR = sizeof(_grammerTable_LR) / (3 * sizeof(char *));

    // lube the tree
    // NOTE(Noah): ensures that when creating regex trees, the grammer is "well defined".
    {
        for ( unsigned int i = 0; i < grammerCount; i++) {
            GRAMMER.AddDef(_grammerTable[i][0]);
        }

        for (unsigned int i = 0; i < grammerCountLR; i++) {
            GRAMMER.AddDef(_grammerTable_LR[i][0]);
        }
    }

    // define grammers with Regex Trees pre-generated.
    {
        for (unsigned int i = 0; i < grammerCount; i++) {
            GRAMMER.AddDef(_grammerTable[i][0], CreateGrammerDefinition(
                _grammerTable[i][0],
                _grammerTable[i][1]
            ));
        }
        
        // left-recursive grammers
        for (unsigned int i = 0; i < grammerCountLR; i++) {
            GRAMMER.AddDef(_grammerTable_LR[i][0], CreateGrammerDefinition(
                _grammerTable_LR[i][0],
                _grammerTable_LR[i][1],
                _grammerTable_LR[i][2]
            ));
        }
    }
 
    if (VERBOSE) {
        GRAMMER.Print();
    }
}   
#endif