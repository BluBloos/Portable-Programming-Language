from tree import Tree
import lexer

equivalences = {}
equivalences["term"] = ["factor"]
equivalences["additive_exp"] = ["term", "factor"]
equivalences["relational_exp"] = ["additive_exp", "term", "factor"]
equivalences["equality_exp"] = ["relational_exp", "additive_exp", "term", "factor"]
equivalences["logical_and_exp"] = ["equality_exp", "relational_exp", "additive_exp", "term", "factor"]
equivalences["logical_or_exp"] = ["logical_and_exp", "equality_exp", "relational_exp", "additive_exp", "term", "factor"]
equivalences["conditional_exp"] = ["logical_or_exp", "logical_and_exp", "equality_exp", "relational_exp", "additive_exp", "term", "factor"]

# TODO(Noah): Must be some way to simplify this.
def IsValidType(typeStr):
    return typeStr in ["float", "double", "int", "char", "short", "string"]

# NOTE(Noah): Grammer definition are custom regular expressions that I invented,
# regardless of if there are parsing libraries out there...
class GrammerDefinition:
    def __init__(self, name, regExp):
        self.name = name
        self.regExp = regExp
class Grammer:
    def __init__(self):
        self.defs = {}

'''
    Tree examples
    r"[(function_decl)(function_impl)(var_decl)(struct_decl)]*"
    ->
    Root
        Any, *
            function_decl
            function_impl
            var_decl
            struct_decl

    r"(type)(symbol)\(((lv),)*(lv)?\)[;(block)]"
    ->
    Root
        type
        symbol
        (
        Group*
            lv
            ,
        lv?
        )
        Any
            ;
            block
    '''
def CreateRegexTree(grammer, regex):
    regexTree = Tree("root")
    contextStack = [regexTree]
    n = 0
    while n < len(regex):
        char = regex[n]
        # enter in and out of context stacks.
        if char == '[':
            contextStack.append( Tree("Any") )
        elif char == '(':
            # Check first if we are looking at a grammer object.
            length, newStr = lexer.searchUntil(regex, n + 1, ')')
            if newStr in grammer.defs.keys() or newStr == "symbol" or newStr == "keyword" or newStr == "literal":
                newTree = Tree(newStr)
                contextStack[-1].Adopt(newTree)
                n += 1 + len(newStr) # skip over () block
            elif newStr.startswith("keyword="):
                newTree = Tree( newStr[8:] )
                contextStack[-1].Adopt(newTree)
                n += 1 + len(newStr) # skip over () block
            elif newStr.startswith("op,"):
                newTree = Tree( "op" + newStr[3:] )
                contextStack[-1].Adopt(newTree)
                n += 1 + len(newStr) # skip over () block
            else:
                contextStack.append( Tree("Group") )
        elif char in '])':
            pop_tree = contextStack.pop(-1)
            contextStack[-1].Adopt(pop_tree)
        elif char in '?+*':
            # NOTE(Noah): This if statement actually never evaluates to true it seems...
            # We can tree the Any/Group simply as children of a tree.
            if len(contextStack[-1].children) == 0:
                # if there are no children, then for sure the last thing added to regex tree
                # was in fact the Any/Group root node itself.  
                contextStack[-1].modifier = char
            else:
                # preceeding_re is assumed to be a child of a tree
                contextStack[-1].children[-1].modifier = char
        elif char == '\\': 
            # the escape sequence:
            # TODO(Noah): Investigage if this is unsafe or something like this...
            contextStack[-1].Adopt(Tree(regex[n+1]))
            n += 1 # make sure to skip over the character that we escaped so we do not double it
        else: 
            # just a character that we are trying to match
            contextStack[-1].Adopt(Tree(char))
        n += 1
    return regexTree

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
    grammer.defs["type"] = GrammerDefinition(
        "type",
        r"[((op,[)(literal)(op,])(type))((op,[])(type))((op,->)(type))(_symbol)(keyword)]"
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
    grammer.defs["statement"]=GrammerDefinition(
        "statement",
        r"[;([(_return)(var_decl)(expression)(_break)(_continue)];)(block)(_if)(_for)(_while)(_switch)]"
    )
    grammer.defs["_return"] = GrammerDefinition(
        "_return",
        r"(keyword=return)(expression)"
    )
    grammer.defs["var_decl"] = GrammerDefinition(
        "var_decl",
        r"(lv)(=(expression))?"
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
    Due to the way that I have defined the grammer for the switch statements,
    we cannot have default: in front of any case:. It must be that default: comes at the end...
    TODO(Noah): Check if this is valid C
    '''
    grammer.defs["_switch"] = GrammerDefinition(
        "_switch",
        r"(keyword=switch)\((expression)\)\{((keyword=case)(expression):(statement)*)*((keyword=default):(statement)*)?\}"
    )
    grammer.defs["expression"] = GrammerDefinition(
        "expression",
        r"[(assignment_exp)(conditional_exp)]"
    )
    grammer.defs["assignment_exp"] = GrammerDefinition(
        "assignment_exp",
        r"(symbol)=(expression)"
    )
    grammer.defs["conditional_exp"] = GrammerDefinition(
        "conditional_exp",
        r"(logical_or_exp)\?(expression):(expression)"
    )
    grammer.defs["logical_or_exp"] = GrammerDefinition(
        "logical_or_exp",
        r"(logical_and_exp)(op,||)(logical_and_exp)"
    )
    grammer.defs["logical_and_exp"] = GrammerDefinition(
        "logical_and_exp",
        r"(equality_exp)(op,&&)(equality_exp)"
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
        r"(factor)([(op,*)(op,/)](factor))+"
    )
    grammer.defs["function_call"] = GrammerDefinition(
        "function_call",
        r"(_symbol)\(((expression)(,(expression))*)?\)"
    )
    grammer.defs["factor"] = GrammerDefinition(
        "factor",
        r"[(literal)(function_call)(_symbol)([(op,!)(op,-)(op,&)(op,*)(\((type)\))](factor))(\((expression)\))]"
    )
    return grammer