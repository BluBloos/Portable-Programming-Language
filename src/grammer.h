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
So basically, the issue here is that for each Grammer.AddDef call, we contruct the RegexTree.
but in doing so, we require to know what type of grammer definitions already exist.
*/

char *_grammerTable[][2] = {
    {   
        "program",
        "[(function)((var_decl);)(struct_decl)]*"
    },
    {   
        "struct_decl",
        "(keyword=struct)(symbol)\\{((var_decl);)*\\};"
    },
    {
        "lv",
        "(type)(symbol)"
    },
    {
        "function",
        "(type)(symbol)\\(((lv)(,(lv))*)?\\)[;(statement)]"
    },
    {
        "_symbol",
        "[((symbol)::(symbol))(symbol)]"
    },
    { 
        "_const",
        "(keyword=const)"
    },
    {
        "_dynamic",
        "(keyword=dynamic)"
    },
    {
        "type",
        "[([((op,[)[(_dynamic)(literal)]?(op,]))(op,->)(_const)](type))(_symbol)(keyword)]"
    },
    {
        "block",
        "\\{(statement)*\\}"
    },
    {
        "_break",
        "(keyword=break)"
    },
    {
        "_continue",
        "(keyword=continue)"
    },
    {
        "_fallthrough",
        "(keyword=fallthrough)"
    },
    {
        "_return",
        "(keyword=return)(expression)"
    },
    {
        "statement",
        "[;([(_return)(var_decl)(expression)(_break)(_continue)(_fallthrough)];)(block)(_if)(_for)(_while)(_switch)]"
    },
    { 
        // TODO(Noah): I would certainly like to remove this grammer definition.
        "statement_noend",
        "[(var_decl)(expression)(_return)(_break)(_continue)(block)(_if)(_for)(_while)(_switch)]"
    },
    {
        "var_decl",
        "(lv)(=[(initializer_list)(expression)])?"
    },
    {
        "_if",
        "(keyword=if)\\((expression)\\)(statement)((keyword=else)(statement))?"
    },
    {
        // NOTE(Noah): Notice that this allows for having for-loops as the end condition
        // of a higher-level for-loop. 
        "_for",
        "(keyword=for)\\((statement)(expression);(statement_noend)\\)(statement)"
    },
    {
        "_while",
        "(keyword=while)\\((expression)\\)(statement)"
    },
    {
        "_switch_default",
        "(keyword=case):(statement)*"
    },
    {
        //NOTE(Noah): Switch statement grammer makes it such that default case MUST come last.
        // Is this intended?
        "_switch",
        "(keyword=switch)\\((expression)\\)\\{((keyword=case)(expression):(statement)*)*(_switch_default)?\\}"
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
        "term",
        "(factor)([(op,*)(op,/)(op,%)](factor))*"
    },
    {
        "function_call",
        "(_symbol)\\(((expression)(,(expression))*)?\\)"
    },
    {
        "_sizeof",
        "(keyword=sizeof)"
    },
    {
        // TODO(Noah): Look into making this grammer definition more readable -> ghost definitions?
        "factor",
        "[(object)((_sizeof)\\([(_symbol)(type)]\\))([(op,!)(op,-)(op,&)(op,*)(op,~)(\\((type)\\))](factor))(\\((expression)\\))]"
    },
    {
        "initializer_list",
        "\\[((literal)(,(literal))*)?\\]"
    }
};

char  *_grammerTable_LR[][3] = {
    {
        "object",
        "[(function_call)(_symbol)(literal)]", // beta
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
        for ( int i = 0; i < grammerCount; i++) {
            GRAMMER.AddDef(_grammerTable[i][0]);
        }

        for (int i = 0; i < grammerCountLR; i++) {
            GRAMMER.AddDef(_grammerTable_LR[i][0]);
        }
    }

    // define grammers with Regex Trees pre-generated.
    {
        for (int i = 0; i < grammerCount; i++) {
            GRAMMER.AddDef(_grammerTable[i][0], CreateGrammerDefinition(
                _grammerTable[i][0],
                _grammerTable[i][1]
            ));
        }
        
        // left-recursive grammers
        for (int i = 0; i < grammerCountLR; i++) {
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