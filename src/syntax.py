from tree import Tree
import grammer as g
import lexer

# TODO(Noah):
''' Handle the following tree case
[LOG]: Printing REGEX tree for r"(type)(symbol)\(((lv),)*(lv)\)[;(block)]"
[LOG]: root
[LOG]:   type
[LOG]:   symbol
[LOG]:   (
[LOG]:   Group*
[LOG]:     lv
[LOG]:     ,
[LOG]:   lv
[LOG]:   )
[LOG]:   Any
[LOG]:     ;
[LOG]:     block
'''

def ParseWithRegexTree(tokens, grammer, regexTree, logger):
    treesParsed = []
    fail_flag = False

    # NOTE(Noah): Was going to make a ParseWithRegexTreeAny
    # function. But this is just same func with knowledge of parent.
    # Easy, we just check if regexTree is an Any.
    any_flag = "Any" in regexTree.data 
    k = 0

    while k < len(regexTree.children):
        child = regexTree.children[k]

        # TODO(Noah): Fix error where regexTree includes * as a data item
        # which then gets interpreted as a false modifier. This is simply an
        # operation.

        # find the modifier, which applies to any of these!
        modifier = child.modifier
        #possibleModifier = child.data[-1]
        #if possibleModifier in "?*+":
        #    modifier = possibleModifier

        child_data = child.data
        #if modifier != None:
        #    # trim modifier on child data
        #    child_data = child_data[:-1]

        # None means 1 and exactly 1.
        # ? is the 0 or 1 modifier
        # * is the 0 or many modifier
        # + is the 1 or many modifier

        n = 0
        while_cond = lambda n : True 
        if modifier == '?' or modifier == None:
            while_cond = lambda n : n < 1

        re_matched = 0

        while while_cond(n):   
            if child_data == "Group":
                trees, flag = ParseWithRegexTree(tokens, grammer, child, logger)
                if not flag:
                    re_matched += 1
                    for t in trees:
                        treesParsed.append(t)
                else:
                    break
            elif child_data == "Any":
                trees, flag = ParseWithRegexTree(tokens, grammer, child, logger)
                if not flag:
                    re_matched += 1
                    for t in trees:
                        treesParsed.append(t)
                else:
                    break
            else:
                # Is this a grammer element?
                if child_data in grammer.defs.keys():
                    treeChild = ParseTokensWithGrammer(tokens, 
                        grammer, grammer.defs[child_data], logger)
                    if treeChild:
                        treesParsed.append(treeChild)
                        re_matched += 1
                    else:
                        # TODO(Noah): This point here is potentially where
                        # we want to include logger statements.
                        break # didn't find grammer object we wanted.
                # TODO(Noah): Factor literal and symbol into one thing because they basically do 
                # the same thing...
                elif child_data == "literal":
                    token = tokens.QueryNext() # the whole LR k+1 idea :)
                    if token.type == "LITERAL":
                        tokens.Next()
                        treesParsed.append(Tree("LITERAL:"+token.value))
                        re_matched += 1
                    else:
                        break
                elif child_data == "symbol":
                    token = tokens.QueryNext() # the whole LR k+1 idea :)
                    if token.type == "SYMBOL":
                        tokens.Next()
                        treesParsed.append(Tree("SYMBOL:"+token.value))
                        re_matched += 1
                    else:
                        break # expected symbol, didn't get it.
                elif child_data.startswith("op"):
                    op = child_data[2:]
                    matched = True
                    for char in op:
                        token = tokens.QueryNext()
                        if token.type == "OP" and token.value == char:
                            tokens.Next()
                            token = tokens.QueryNext()
                        else:
                            matched = False
                            break
                    if matched:
                        treesParsed.append(Tree("OP:"+op))
                        re_matched += 1
                    else:
                        break # expected specific op, didn't get it.
                else:
                    # Single character to match, presumably?
                    token = tokens.QueryNext()
                    if token.value == child_data:
                        tokens.Next()
                        re_matched += 1
                        # NOTE(Noah): Simply consume character,
                        # no Tree to add for this one.
                    else:
                        break # didn't match character
            
            n += 1
        
        # Find all fail cases.
        fail_flag = (modifier == None and re_matched != 1) or \
           (modifier == '?' and re_matched > 1) or \
           (modifier == '+' and re_matched == 0)
        
        # NOTE(Noah): Below is a logicial xor.
        if fail_flag and not any_flag or any_flag and not fail_flag:
            break
    
        k += 1
        
    return (treesParsed, fail_flag)

'''
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
'''
def ParseTokensWithGrammer(tokens, grammer, grammerDef, logger):
    tree = Tree(grammerDef.name)
    regex = grammerDef.regExp
    regexTree = g.CreateRegexTree(grammer, regex)
    
    trees, fail_flag = ParseWithRegexTree(tokens, grammer, regexTree, logger)
    # NOTE(Noah): I know this is going to work and do what I want it to do, but 
    # I would say that there MUST be a better way to do this...
    # Namely I do not like len(trees) == 1...
    if fail_flag and len(trees) == 1:
        if g.equivalences[grammerDef.name]:
            for eq in g.equivalences[grammerDef.name]:
                if eq == trees[0].data:
                    fail_flag = False # redemption.
                    return trees[0] # override upper level grammer object. Makes AST simpler.

    if not fail_flag:
        for t in trees:
            tree.Adopt(t)
    else:
        return False # Fail condition. We either get the grammer object or we do not...

    return tree


def Run(tokens, logger):
    grammer = g.LoadGrammer()
    abstractSyntaxTree = ParseTokensWithGrammer(tokens, grammer, grammer.defs["program"], logger)
    # We want to check if the syntax parser failed, which is to say that the code has improper grammar.
    return abstractSyntaxTree