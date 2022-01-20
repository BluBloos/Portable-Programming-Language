#ifndef GRAMMER_H
#define GRAMMER_H
#include <tree.h>

// TODO(Noah): If we want to make our AST tree dramatically simpler, 
// we can inline regex. Then we can make it highly modular (human readable),
// but also have it so that it constructs a well optimized AST.

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
    std::unordered_map<const char *, struct grammer_definition> defs;
    bool DefExists(const char *defName) {
        // TODO(Noah): Implement.
        return defs.find(defName) != defs.end();
    }
    // NOTE(Noah): AddDef doesn't check to ensure that the defName you supplied already exists.
    // It assumes this responsibility to the caller.
    void AddDef(const char *defName, struct grammer_definition gd) {
        defs[defName] = gd;
    }
    // TODO(Noah): Implement this.
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

// TODO(Noah): Account for failure to generate RegexTree.
struct tree_node CreateRegexTree(Grammer &grammer, const char *regex) {
    
    struct tree_node regexTree = CreateTree(TREE_ROOT);
    std::vector<struct tree_node> contextStack(1);
    contextStack[0] = regexTree;

    int regexLen = strlen(regex);
    int n = 0;

    while (n < regexLen) {
        char c = regex[n]; 
        struct tree_node &lastTree = contextStack[contextStack.size()-1];
        // Enter in and out of context stacks.
        if (c == '[') {
            contextStack.push_back(CreateTree(TREE_REGEX_ANY));
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
                struct tree_node tn = CreateTree(TREE_REGEX_STR, (newStr.c_str()+8) );
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else if (SillyStringStartsWith(newStr.c_str(), "op,")) {
                struct tree_node tn = CreateTree(TREE_REGEX_STR, (newStr.c_str()+3) );
                TreeAdoptTree(lastTree, tn);
                n += 1 + newStr.size(); // skip over () block
            } else {
                contextStack.push_back(CreateTree(TREE_REGEX_GROUP));
            }
        } 
        else if (c == ']' || c == ')') {
            contextStack.pop_back();
            TreeAdoptTree(contextStack[contextStack.size() - 1], lastTree);
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
    return contextStack[0];
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

// Loads the grammer into the global grammer object.
void LoadGrammer() {
    GRAMMER.AddDef("program", CreateGrammerDefinition(
        "program",
        "[(function)((var_decl);)(struct_decl)]*"
    ));
    if (VERBOSE) {
        GRAMMER.Print();
    }
}

/*
def LoadGrammer():
    grammer = Grammer()
    grammer.defs["program"] = GrammerDefinition(
        "program",
        r"[(function)((var_decl);)(struct_decl)]*"
    )
    grammer.defs["struct_decl"] = GrammerDefinition(
        "struct_decl",
        r"(keyword=struct)(symbol)\{((var_decl);)*\};"
    )
    grammer.defs["lv"] = GrammerDefinition(
        "lv",
        r"(type)(symbol)"
    )
    grammer.defs["function"]=GrammerDefinition(
        "function",
        r"(type)(symbol)\(((lv)(,(lv))*)?\)[;(statement)]"
    )
    grammer.defs["_symbol"] = GrammerDefinition(
        "_symbol",
        r"[((symbol)::(symbol))(symbol)]"
    )
    grammer.defs["_const"] = GrammerDefinition(
        "_const",
        r"(keyword=const)"
    )
    grammer.defs["_dynamic"] = GrammerDefinition(
        "_dynamic",
        r"(keyword=dynamic)"
    )
    grammer.defs["type"] = GrammerDefinition(
        "type",
        r"[([((op,[)[(_dynamic)(literal)]?(op,]))(op,->)(_const)](type))(_symbol)(keyword)]"
    )
    grammer.defs["block"]=GrammerDefinition(
        "block",
        r"\{(statement)*\}"
    )
    grammer.defs["_break"]=GrammerDefinition(
        "_break",
        r"(keyword=break)"
    )
    grammer.defs["_continue"]=GrammerDefinition(
        "_continue",
        r"(keyword=continue)"
    )
    grammer.defs["_fallthrough"]=GrammerDefinition(
        "_fallthrough",
        r"(keyword=fallthrough)"
    )
    grammer.defs["statement"]=GrammerDefinition(
        "statement",
        r"[;([(_return)(var_decl)(expression)(_break)(_continue)(_fallthrough)];)(block)(_if)(_for)(_while)(_switch)]"
    )
    grammer.defs["_return"] = GrammerDefinition(
        "_return",
        r"(keyword=return)(expression)"
    )
    # NOTE(Noah): Right now, the only place we allow the use of initializer lists is to init a variable.
    # It is likely that we will extend this in the future.
    grammer.defs["var_decl"] = GrammerDefinition(
        "var_decl",
        r"(lv)(=[(initializer_list)(expression)])?"
    )
    grammer.defs["_if"] = GrammerDefinition(
        "_if",
        r"(keyword=if)\((expression)\)(statement)((keyword=else)(statement))?"
    )
    grammer.defs["statement_noend"] = GrammerDefinition(
        "statement_noend",
        r"[(var_decl)(expression)(_return)(_break)(_continue)(block)(_if)(_for)(_while)(_switch)]"
    )
    # NOTE(Noah): Noticing that this allows for having for-loops as the end condition
    # of a higher-level for-loop. I guess that is not so bad LOL.
    # Of course, we can ensure the validity of the tree after it is made.
    # TODO(Noah): Maybe not allow silly things like this.    
    grammer.defs["_for"] = GrammerDefinition(
        "_for",
        r"(keyword=for)\((statement)(expression);(statement_noend)\)(statement)"
    )
    grammer.defs["_while"] = GrammerDefinition(
        "_while",
        r"(keyword=while)\((expression)\)(statement)"
    )
    '''
    NOTE(Noah): Switch statement grammer makes it such that default case MUST come last.
    '''
    grammer.defs["_switch_default"] = GrammerDefinition(
        "_switch_default",
        r"(keyword=case):(statement)*"
    )
    grammer.defs["_switch"] = GrammerDefinition(
        "_switch",
        r"(keyword=switch)\((expression)\)\{((keyword=case)(expression):(statement)*)*(_switch_default)?\}"
    )
    grammer.defs["expression"] = GrammerDefinition(
        "expression",
        r"[(assignment_exp)(conditional_exp)]"
    )
    grammer.defs["assignment_exp"] = GrammerDefinition(
        "assignment_exp",
        r"(factor)[(op,=)(op,+=)(op,-=)(op,*=)(op,/=)(op,%=)(op,&=)(op,|=)](expression)"
    )
    grammer.defs["conditional_exp"] = GrammerDefinition(
        "conditional_exp",
        r"(logical_or_exp)\?(expression):(expression)"
    )
    grammer.defs["logical_or_exp"] = GrammerDefinition(
        "logical_or_exp",
        r"(logical_and_exp)[(op,||)(logical_and_exp)]+"
    )
    grammer.defs["logical_and_exp"] = GrammerDefinition(
        "logical_and_exp",
        r"(bitwise_or_exp)(op,&&)(bitwise_or_exp)"
    )
    grammer.defs["bitwise_or_exp"] = GrammerDefinition(
        "bitwise_or_exp",
        r"(bitwise_and_exp)(op,|)(bitwise_and_exp)"
    )
    grammer.defs["bitwise_and_exp"] = GrammerDefinition(
        "bitwise_and_exp",
        r"(equality_exp)(op,&)(equality_exp)"
    )
    grammer.defs["equality_exp"] = GrammerDefinition(
        "equality_exp",
        r"(relational_exp)[(op,==)(op,!=)](relational_exp)"
    )
    grammer.defs["relational_exp"] = GrammerDefinition(
        "relational_exp",
        r"(additive_exp)[(op,>=)(op,<=)(op,<)(op,>)](additive_exp)"
    )
    grammer.defs["additive_exp"] = GrammerDefinition(
        "additive_exp",
        r"(term)([(op,+)(op,-)](term))+"
    )
    grammer.defs["term"] = GrammerDefinition(
        "term",
        r"(factor)([(op,*)(op,/)(op,%)](factor))+"
    )
    grammer.defs["function_call"] = GrammerDefinition(
        "function_call",
        r"(_symbol)\(((expression)(,(expression))*)?\)"
    )
    grammer.defs["_sizeof"] = GrammerDefinition(
        "_sizeof",
        r"(keyword=sizeof)"
    )
    # NOTE(Noah): This grammer does get the right-to-left associativity down, despite me not thinking about it.
    # Good thing we followed that tutorial long ago.
    grammer.defs["factor"] = GrammerDefinition(
        "factor",
        r"[(object)((_sizeof)\([(_symbol)(type)]\))([(op,!)(op,-)(op,&)(op,*)(op,~)(\((type)\))](factor))(\((expression)\))]"
    )
    # NOTE(Noah): I really don't know if these initializer lists should have like, more than just literals?
    # But for now this is okay!
    grammer.defs["initializer_list"] = GrammerDefinition(
        "initializer_list",
        r"\[((literal)(,(literal))*)?\]"
    )
    # TODO(Noah): Prob take out \((expression)\) and put in the factor. Would make the resulting AST simpler.
    # NOTE(Noah): Right now just making ++ and -- postfix ops act on a _symbol. Makes the most semantics sense.
    # Maybe might need to change the grammer later but this is the simplest thing.
    grammer.defs["object"] = GrammerDefinition(
        "object",
        beta=r"[(function_call)(_symbol)(literal)]",
        alpha=r"[(op,++)(op,--)((op,[)(expression)(op,]))((op,.)(object))]",
        type="left-recursive"
    )
    return grammer
*/
#endif GRAMMER_H