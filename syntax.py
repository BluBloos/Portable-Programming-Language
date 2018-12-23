from tree import Tree

'''
class Node:
    def __init__(self, nodes, annotation):
        #NOTE(Noah): Currently, we don't support more than one child node
        self.children = nodes
        self.NULL = 0
        self.annotation = annotation
    def NextNode(self):
        if type(self.children[0]) == Node:
            return self.children[0]
        else:
            return self.NULL
    def GetChildren(self):
        children = []
        for child in self.children:
            if type(child) == Node:
                children.append(child)
        if len(children) > 0:
            return children
        return False
    def Climb(self):
        result = []
        currentNode = self
        while currentNode:
            result.append(currentNode)
            currentNode = currentNode.NextNode()
        result = result[1:]
        return result

class AbstractSyntaxTree:
    def __init__(self, rootNode):
        self.rootNode = rootNode
    def DebugPrint(self, logger):
        currentNode = self.rootNode
        logger.Log("Root: " + currentNode.annotation)
        self.DebugSearch(logger, currentNode.GetChildren(), 1)

    def DebugSearch(self, logger, children, indentation):
        if children:
            #indentation += 1
            for child in children:
                logger.Log(" " * indentation + "Node: " + str(child.annotation))
                self.DebugSearch(logger, child.GetChildren(), indentation)
'''

def IsInteger(string):
    for char in string:
        if char not in "0123456789":
            return False
    return True

def IsUnary(char):
    if char in "!-":
        return True
    return False

def IsBinary(char):
    if char in "+-/*":
        return True
    return False

def Parse_Factor(tokens, logger):
    #the first token can be a parenthesis, a unary op, or an int
    token = tokens.Next()
    if token.type == "SYMBOL":
        if not IsInteger(token.value):
            logger.Error("line " + str(token.line) + " expected integer")
            return (False, tokens)
        return (Tree(token.value), tokens)
    elif token.type == "OP" and IsUnary(token.value):
        #the expression is a unary expression
        factor, tokens = Parse_Factor(tokens, logger)
        if not factor:
            return (False, tokens)
        return (Tree(token.value, [factor]), tokens)
    elif token.type == "PART" and token.value == "(":
        #the next token must be an expression
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        #then there must be another bracket
        token = tokens.Next()
        if not (token.type == "PART" and token.value == ")"):
            logger.Error("line " + str(token.line) + " expected )")
            return (False, tokens)
        return (expression, tokens)
    else:
        logger.Error("line " + str(token.line) + " expected unary, literal, or (")
        return (False, tokens)

def Parse_Term(tokens, logger):
    #the first token must be a factor
    factor, tokens = Parse_Factor(tokens, logger)
    if not factor:
        return (False, tokens)

    token = tokens.Query()
    lastFactor = factor
    while(token.type == "OP" and (token.value == "*" or token.value == "/")):
        token = tokens.Next() #actually advance
        factor, tokens = Parse_Factor(tokens, logger)
        if not factor:
            return (False, tokens)
        lastFactor = Tree(token.value, [lastFactor, factor])
        token = tokens.Query()

    return (lastFactor, tokens)

def Parse_Expression(tokens, logger):
    #the first token must be a term
    term, tokens = Parse_Term(tokens, logger)
    if not term:
        return (False, tokens)

    token = tokens.Query()
    lastTerm = term
    while(token.type == "OP" and (token.value == "+" or token.value == "-")):
        token = tokens.Next() #actually advance
        term, tokens = Parse_Term(tokens, logger)
        if not term:
            return (False, tokens)
        lastTerm = Tree(token.value, [lastTerm, term])
        token = tokens.Query()

    return (lastTerm, tokens)

def Parse_Statement(tokens, logger):
    token = tokens.Next()
    if not (token.type == "KEY" and token.value == "return"):
        logger.Error("line " + str(token.line) + " expected return")
        return (False, tokens)

    expression, tokens = Parse_Expression(tokens, logger)
    if not expression:
        return (False, tokens)

    token = tokens.Next()
    if not token.type == "END":
        logger.Error("line " + str(token.line) + " expected END")
        return (False, tokens)

    return (Tree("return", [expression]), tokens)

def Parse_Function(tokens, logger):
    token = tokens.Next()
    if not (token.type == "KEY" and token.value == "int"):
        logger.Error("line " + str(token.line) + " expected int")
        return (False, tokens)

    token = tokens.Next()
    if not token.type == "SYMBOL":
        logger.Error("line " + str(token.line) + " expected symbol")
        return (False, tokens)
    name = token.value

    token = tokens.Next()
    if not (token.type == "PART" and token.value == "("):
        logger.Error("line " + str(token.line) + " expected (")
        return (False, tokens)

    token = tokens.Next()
    if not (token.type == "PART" and token.value == ")"):
        logger.Error("line " + str(token.line) + " expected )")
        return (False, tokens)

    token = tokens.Next()
    if not (token.type == "PART" and token.value == "{"):
        logger.Error("line " + str(token.line) + " expected {")
        return (False, tokens)

    statement, tokens = Parse_Statement(tokens, logger)
    if not statement:
        return (False, tokens)

    token = tokens.Next()
    if not (token.type == "PART" and token.value == "}"):
        logger.Error("line " + str(token.line) + " expected }")
        return (False, tokens)

    return (Tree(name, [statement]), tokens)

def Parse_Program(tokens, logger):
    function, tokens = Parse_Function(tokens, logger)
    if not function:
        return (False, tokens)
    return (Tree("program", [function]), tokens)

def Run(tokens, logger):
    program, tokens = Parse_Program(tokens, logger)
    return program
