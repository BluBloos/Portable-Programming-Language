from tree import Tree
import grammer as g
import lexer

'''
Trying to reduce the error count and get GOOD errors.
Like, would be massively helpful.

Starting error count: 223.
Pass #1: 273 (Added which grammer object was failed + null errors resolved by grammer equivalency)
Pass #2: 250 (null errors for Any block to only include prints from last item in Any)
Pass #3: 250 (null errors for * modifier)
Pass #4: 0 (added buffering to ParseTokensWithGrammer)
Pass #5: 0 (snake_game.c)
Pass #6: 3 (error pass thru for top level program)

# TODO(Noah): Error printing for the AST generation.

Modifiers
    None -> 1 and exactly 1. Print all errors.
    ? -> 0 or 1. Error prints if we find more than expected. In which case we state it as it is.
- Only take the errors from the last element in a group.
    
# TODO: For an Any block, pick one path. Just pick one of the k frames.
    - that K frame should be the one with the MOST errors??

# TODO(Noah): Presumably done in the land of printing errors for AST generation.
Modifiers:
    * -> 0 or more. Never print errors for these.
    + -> 1 or more. If we get 0, we print "Expected ___".
Groups
    - Print errors as they are seen. No restrictions.
   
Any blocks
    - 


Fail case resolved by Grammer equivalency.
    - Null Prints.
specific errors <- the actual fail points. Token mismatch.
'''

class AST_Error:
    def __init__(self, _str, line_number, token_index):
        self._str = _str
        self.line_number = line_number
        self.token_index = token_index
    def __str__(self):
        return "syntax error on line {}. {}".format(self.line_number, self._str)

def ParseWithRegexTree(tokens, grammer, regexTree, logger, grammerDefName=""):
    treesParsed = []
    fail_flag = False
    buffered_errors = [] # Frames for each recursive func call.

    # NOTE(Noah): Was going to make a ParseWithRegexTreeAny
    # function. But this is just same func with knowledge of parent.
    # Easy, we just check if regexTree is an Any.
    any_flag = "Any" in regexTree.data 
    group_flag = "Group" in regexTree.data
    k = 0

    # presuming that we are inside of an Any block, we want to preserve state.
    # to rollback tokens upon failure.
    # NOTE(Noah): We also want to rollback tokens when we tried to parse a group with a modifier
    #  on the group.
    tokens_savepoint = None
    if any_flag:
        tokens_savepoint = tokens.GetSavepoint()

    while k < len(regexTree.children):

        if group_flag:
            # clear errors from last k.
            buffered_errors = []

        child = regexTree.children[k]
        # find the modifier, which applies to any of these!
        modifier = child.modifier
        child_data = child.data

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
            if child_data == "Group" or child_data == "Any":
                trees, err_flag, _buffered_errors = ParseWithRegexTree(tokens, grammer, child, logger)
                if not err_flag:
                    re_matched += 1
                    for t in trees:
                        treesParsed.append(t)
                else:
                    buffered_errors += (_buffered_errors)  
                    break
            # Is this a grammer element?
            elif child_data in grammer.defs.keys():
                treeChild, _buffered_errors = ParseTokensWithGrammer(tokens, \
                    grammer, grammer.defs[child_data], logger)
                if treeChild:
                    treesParsed.append(treeChild)
                    re_matched += 1
                else:
                    buffered_errors += (_buffered_errors)
                    break # didn't find grammer object we wanted.
            # TODO(Noah): Factor literal and symbol into one thing because they basically do 
            # the same thing...
            elif child_data == "literal":
                token = tokens.QueryNext() # the whole LR k+1 idea :)
                if token.type == "LITERAL" or token.type == "QUOTE" or token.type == "C_LITERAL":
                    tokens.Next()
                    if token.type == "C_LITERAL":
                        treesParsed.append(Tree("C_LITERAL:"+token.value))
                    else:
                        treesParsed.append(Tree("LITERAL:"+token.value))
                    re_matched += 1
                else:
                    buffered_errors.append( 
                        AST_Error(
                            "Expected LITERAL but got '{}'".format(token.value), 
                            token.line,
                            tokens.GetSavepoint() ))
                    break
            elif child_data == "symbol":
                token = tokens.QueryNext() # the whole LR k+1 idea :)
                if token.type == "SYMBOL":
                    tokens.Next()
                    treesParsed.append(Tree("SYMBOL:"+token.value))
                    re_matched += 1
                else:
                    buffered_errors.append(AST_Error(
                            "Expected SYMBOL but got '{}'".format(token.value), 
                            token.line,
                            tokens.GetSavepoint() ))
                    break # expected symbol, didn't get it.
            elif child_data.startswith("op"):
                op = child_data[2:]
                token = tokens.QueryNext()
                if token.type == "OP" and token.value == op:
                    tokens.Next()
                    treesParsed.append(Tree("OP:"+op))
                    re_matched += 1
                else:
                    buffered_errors.append(AST_Error(
                            "Expected op={} but got '{}'".format(op, token.value), 
                            token.line,
                            tokens.GetSavepoint() ))
                    #logger.Error("Line {}. Expected op={}".format())
                    break # expected specific op, didn't get it.
                '''
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
                '''
                
            elif child_data == "keyword":
                token = tokens.QueryNext()
                if token.type == "KEY":
                    tokens.Next()
                    treesParsed.append(Tree("KEY:"+token.value))
                    re_matched += 1
                else:
                    # TODO(Noah): Factor out this AST_Error generation. Quite similar in all cases.
                    buffered_errors.append(AST_Error(
                            "Expected KEYWORD but got '{}'".format(token.value), 
                            token.line,
                            tokens.GetSavepoint() ))
                    break
            else:
                # Single character to match, presumably?
                # NOTE(Noah): Experimentally untrue
                token = tokens.QueryNext()
                if token.value == child_data:
                    tokens.Next()
                    re_matched += 1
                    # NOTE(Noah): Simply consume character,
                    # no Tree to add for this one.
                else:
                    buffered_errors.append(AST_Error(
                            "Expected '{}' but got '{}'".format(child_data, token.value), 
                            token.line,
                            tokens.GetSavepoint() ))
                    break # didn't match character
            
            n += 1
        
        # Find all fail cases.
        fail_flag = (modifier == None and re_matched != 1) or \
           (modifier == '?' and re_matched > 1) or \
           (modifier == '+' and re_matched == 0)
        
        # NOTE(Noah): Below is a logicial xor.
        if fail_flag and not any_flag or any_flag and not fail_flag:
            break
    
        # TODO(Noah): An alterior method than this is to just always query if under an Any block...    
        if any_flag:
            # presuming we have not yet succeeded, and we are inside an Any block
            # this means we are trying the next child.
            # we need to reset the state of Tokens.
            tokens.ResetSavepoint(tokens_savepoint)    

        k += 1
        
    return (treesParsed, fail_flag, buffered_errors)

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
    buffered_errors = []
    tree = Tree(grammerDef.name)
    regex = grammerDef.regExp
    regexTree = g.CreateRegexTree(grammer, regex)
    
    trees, fail_flag, _buffered_errors = ParseWithRegexTree(tokens, grammer, regexTree, logger, grammerDef.name)
    
    if grammerDef.name == "program":
        buffered_errors += (_buffered_errors)
    
    # NOTE(Noah): I know this is going to work and do what I want it to do, but 
    # I would say that there MUST be a better way to do this...
    # Namely I do not like len(trees) == 1...
    if fail_flag and len(trees) == 1:
        if grammerDef.name in g.equivalences.keys():
            for eq in g.equivalences[grammerDef.name]:
                if eq == trees[0].data:
                    fail_flag = False # redemption.
                    return (trees[0], buffered_errors) # override upper level grammer object. Makes AST simpler.

    if not fail_flag:
        for t in trees:
            tree.Adopt(t)
        # Flush the recordings. Do not care about any errors before this point.
        # logger.ClearRecording()
    else:

        # Print all buffered errors.
        #savepnt = tokens.GetSavepoint()
        # token = tokens.tokens[savepnt]
        #__buffered_errors = [ AST_Error(
        #                    "Unable to parse grammer={}".format(grammerDef.name), 
        #                    token.line,
        #                     savepnt) ]
        __buffered_errors = (_buffered_errors)
        
        return (False, __buffered_errors) # Fail condition. We either get the grammer object or we do not...

    return (tree, buffered_errors)


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