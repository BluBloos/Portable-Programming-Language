from tree import Tree
import traceback


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
    if char in "+-/*%":
        return True
    return False

def Parse_Factor(tokens, logger):
    token = tokens.Next()
    if token.type == "SYMBOL": #check if a literal, a variable, or a function call
        if IsInteger(token.value):
            return (Tree(token.value, 0), tokens)
        #NOTE(Noah): beware of what "not being an integer" means...
        else:
            symbol = token.value
            token = tokens.Query()
            tree = Tree("call:" + symbol, 0)
            if token.type == "PART" and token.value == "(":
                token = tokens.Next() #actually advance
                token = tokens.Query()
                if not (token.type == "PART" and token.value == ")"):
                    expression, tokens = Parse_Expression(tokens, logger)
                    if not expression:
                        return (False, tokens)
                    tree.Adopt(expression)
                    token = tokens.Query()
                    while(token.type == "PART" and token.value == ","):
                        token = tokens.Next()
                        expression, tokens = Parse_Expression(tokens, logger)
                        if not expression:
                            return (False, tokens)
                        #params.append(token.value)
                        tree.Adopt(expression)
                        token = tokens.Query()
                token = tokens.Next()
                if not (token.type == "PART" and token.value == ")"):
                    logger.Error("line " + str(token.line) + " expected )")
                    return (False, tokens)
                return (tree, tokens)

            else:
                return (Tree("var:" + symbol, 0), tokens)

    elif token.type == "OP" and IsUnary(token.value): #check if <unary> <factor>
        #the expression is a unary expression
        factor, tokens = Parse_Factor(tokens, logger)
        if not factor:
            return (False, tokens)
        result = Tree(token.value, 0)
        result.Adopt(factor)
        return (result, tokens)

    elif token.type == "PART" and token.value == "(": #check if (<exp>)
        #the next token must be an expression
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        #then there must be another bracket
        token = tokens.Next()
        if not (token.type == "PART" and token.value == ")"):
            logger.Error("line " + str(token.line) + " expected )")
            for line in traceback.format_stack():
                logger.Log(line.strip())
            return (False, tokens)
        return (expression, tokens)

    elif token.type == "KEY" and token.value == "true":
        return (Tree("1", 0), tokens)

    elif token.type == "KEY" and token.value == "false":
        return (Tree("0", 0), tokens)

    else:
        logger.Error("line " + str(token.line) + " expected factor")
        return (False, tokens)

def Parse_Term(tokens, logger):
    #print("term")
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
        tree = Tree(token.value, 0)
        tree.Adopt(lastFactor)
        tree.Adopt(factor)
        lastFactor = tree
        token = tokens.Query()

    return (lastFactor, tokens)

def Parse_Additive_Expression(tokens, logger):
    #print("Parse_Additive")
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
        tree = Tree(token.value, 0)
        tree.Adopt(lastTerm)
        tree.Adopt(term)
        lastTerm = tree
        token = tokens.Query()

    return (lastTerm, tokens)

def Parse_Relational_Operator(tokens):
    #it can be <, >, <=, >=
    #the function should return how many times to advance, and also the operator
    token1 = tokens.Query()
    if token1.type == "OP" and (token1.value == "<" or token1.value == ">"):
        token1 = tokens.Next()
        token2 = tokens.Query()
        if token2.type == "OP" and token2.value == "=":
            token2 = tokens.Next()
            return (token1.value + token2.value, tokens)
        else:
            return (token1.value, tokens)
    else:
        return (False, tokens)

def Parse_Relational_Expression(tokens, logger):
    #print("Parse_Relational_Expression")
    term, tokens = Parse_Additive_Expression(tokens, logger)
    if not term:
        return (False, tokens)

    lastTerm = term
    op, tokens = Parse_Relational_Operator(tokens)
    #print("op: " + str(op))
    while op:
        term, tokens = Parse_Additive_Expression(tokens, logger)
        if not term:
            return (False, tokens)
        tree = Tree(op, 0)
        tree.Adopt(lastTerm)
        tree.Adopt(term)
        lastTerm = tree
        op, tokens = Parse_Relational_Operator(tokens)

    return (lastTerm, tokens)

def Parse_Equality_Operator(tokens):
    token1 = tokens.Query()
    if token1.type == "OP" and (token1.value == "!" or token1.value == "="):
        token1 = tokens.Next()
        token2 = tokens.Query()
        if token2.type == "OP" and token2.value == "=":
            token2 = tokens.Next()
            return (token1.value + token2.value, tokens)
        else:
            return (token1.value, tokens)
    else:
        return (False, tokens)

def Parse_Equality_Expression(tokens, logger):
    term, tokens = Parse_Relational_Expression(tokens, logger)
    if not term:
        return (False, tokens)

    lastTerm = term
    op, tokens = Parse_Equality_Operator(tokens)
    #print("op: " + str(op))
    while op:
        term, tokens = Parse_Relational_Expression(tokens, logger)
        if not term:
            return (False, tokens)
        tree = Tree(op, 0)
        tree.Adopt(lastTerm)
        tree.Adopt(term)
        lastTerm = tree
        op, tokens = Parse_Equality_Operator(tokens)

    return (lastTerm, tokens)

def Parse_Logical_And_Operator(tokens):
    token1 = tokens.Query()
    if token1.type == "OP" and token1.value == "&":
        token1 = tokens.Next()
        token2 = tokens.Query()
        if token2.type == "OP" and token2.value == "&":
            token2 = tokens.Next()
            return ("&&", tokens)
        else:
            return (False, tokens)
    else:
        return (False, tokens)

def Parse_Logical_And_Expression(tokens, logger):
    term, tokens = Parse_Equality_Expression(tokens, logger)
    if not term:
        return (False, tokens)

    lastTerm = term
    op, tokens = Parse_Logical_And_Operator(tokens)
    #print("op: " + str(op))
    while op:
        term, tokens = Parse_Equality_Expression(tokens, logger)
        if not term:
            return (False, tokens)
        tree = Tree(op, 0)
        tree.Adopt(lastTerm)
        tree.Adopt(term)
        lastTerm = tree
        op, tokens = Parse_Logical_And_Operator(tokens)

    return (lastTerm, tokens)

def Parse_Logical_Or_Operator(tokens):
    token1 = tokens.Query()
    if token1.type == "OP" and token1.value == "|":
        token1 = tokens.Next()
        token2 = tokens.Query()
        if token2.type == "OP" and token2.value == "|":
            token2 = tokens.Next()
            return ("||", tokens)
        else:
            return (False, tokens)
    else:
        return (False, tokens)

def Parse_Logical_Or_Expression(tokens, logger):
    #print("expression")
    term, tokens = Parse_Logical_And_Expression(tokens, logger)
    if not term:
        return (False, tokens)

    lastTerm = term
    op, tokens = Parse_Logical_Or_Operator(tokens)
    #print("op: " + str(op))
    while op:
        term, tokens = Parse_Logical_And_Expression(tokens, logger)
        if not term:
            return (False, tokens)
        tree = Tree(op, 0)
        tree.Adopt(lastTerm)
        tree.Adopt(term)
        lastTerm = tree
        op, tokens = Parse_Logical_Or_Operator(tokens)

    return (lastTerm, tokens)

def Parse_Conditional_Expression(tokens, logger):
    term, tokens = Parse_Logical_Or_Expression(tokens, logger)
    if not term:
        return (False, tokens)

    token = tokens.Query()
    if token.type == "OP" and token.value == "?":
        tree = Tree("conditional", 0)
        tree.Adopt(term)
        token = tokens.Next()
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        tree.Adopt(expression)

        token = tokens.Next()
        if not (token.type == "PART" and token.value == ":"):
            logger.Error("line " + str(token.line) + " expected :")
            return (False, tokens)

        conditional, tokens = Parse_Conditional_Expression(tokens, logger)
        if not conditional:
            return (False, tokens)
        tree.Adopt(conditional)
        return (tree, tokens)

    return (term, tokens)

def Parse_Expression(tokens, logger):
    conditional, tokens = Parse_Conditional_Expression(tokens, logger)
    if not conditional:
        return (False, tokens)
    return (conditional, tokens)

def Parse_Block(tokens, logger, name):
    name += ":block"

    statement, tokens = Parse_Statement(tokens, logger)
    if not statement:
        return (False, tokens)

    #TODO(Noah): I think this while loop can be done a little better
    block = Tree(name, 0)
    while statement:
        block.Adopt(statement)
        token = tokens.Query()
        if not (token.type == "PART" and token.value == "}"):
            statement, tokens = Parse_Statement(tokens, logger)
        else:
            break

    token = tokens.Next()
    if not (token.type == "PART" and token.value == "}"):
        logger.Error("line " + str(token.line) + " expected }")
        return (False, tokens)

    return (block, tokens)

def Parse_Statement(tokens, logger, end=True):
    statement = Tree("statement", 0)
    token = tokens.Next()
    if token.type == "KEY" and token.value == "return": #return statement
        statement.data = "return"
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        statement.Adopt(expression)

    elif token.type == "KEY" and token.value == "int": #variable declaration
        statement.data = "declaration"
        token = tokens.Next()
        if not token.type == "SYMBOL":
            logger.Error("line " + str(token.line) + " expected variable name")
            return (False, tokens)
        statement.GiveBirth(token.value)
        token = tokens.Query()
        if token.type == "OP" and token.value == "=":
            token = tokens.Next()
            expression, tokens = Parse_Expression(tokens, logger)
            if not expression:
                return (False, tokens)
            statement.Adopt(expression)

    elif token.type == "KEY" and token.value == "if": #if statement
        statement.data = "if"
        token = tokens.Next()
        if not (token.type == "PART" and token.value == "("):
            logger.Error("line " + str(token.line) + " expected (")
            return (False, tokens)

        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        statement.Adopt(expression)

        token = tokens.Next()
        if not (token.type == "PART" and token.value == ")"):
            logger.Error("line " + str(token.line) + " expected )")
            return (False, tokens)

        body, tokens = Parse_Statement(tokens, logger)
        if not body:
            return (False, tokens)
        statement.Adopt(body)

        token = tokens.Query()
        if token.type == "KEY" and token.value == "else":
            token == tokens.Next()
            tree = Tree("else", 0)

            body, tokens = Parse_Statement(tokens, logger)
            if not body:
                return (False, tokens)

            tree.Adopt(body)
            statement.Adopt(tree)

        #NOTE(Noah): If statements do not end with a semicolon
        return (statement, tokens)

    elif token.type == "KEY" and token.value == "for":
        statement.data = "for"
        token = tokens.Next()
        if not (token.type == "PART" and token.value == "("):
            logger.Error("line " + str(token.line) + " expected (")
            return (False, tokens)

        init, tokens = Parse_Statement(tokens, logger)
        if not init:
            return (False, tokens)
        statement.Adopt(init)

        condition, tokens = Parse_Expression(tokens, logger)
        if not condition:
            return (False, tokens)
        statement.Adopt(condition)

        token = tokens.Next()
        if not token.type == "END":
            logger.Error("line " + str(token.line) + " expected END")
            for line in traceback.format_stack():
                logger.Log(line.strip())
            return (False, tokens)

        #NOTE(Noah): The following is a bodge. That's because I don't want to make variable declarations expressions.
        end, tokens = Parse_Statement(tokens, logger, False)
        if not end:
            return (False, tokens)
        statement.Adopt(end)

        token = tokens.Next()
        if not (token.type == "PART" and token.value == ")"):
            logger.Error("line " + str(token.line) + " expected )")
            return (False, tokens)

        body, tokens = Parse_Statement(tokens, logger)
        if not body:
            return (False, tokens)
        statement.Adopt(body)
        return (statement, tokens)

    elif token.type == "SYMBOL": #varible assignment
        statement.GiveBirth(token.value)
        statement.data = "assignment"
        token = tokens.Next()
        if not (token.type == "OP" and token.value == "="):
            logger.Error("line " + str(token.line) + " expected =")
            return (False, tokens)
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        statement.Adopt(expression)

    elif token.type == "PART" and token.value == "{": #compound statement
        block, tokens = Parse_Block(tokens, logger, "default")
        if not block:
            return (False, tokens)
        statement = block
        #NOTE(Noah): compound statements do not end with a semicolon
        return (statement, tokens)

    elif token.type == "KEY" and token.value == "break":
        statement.data = "break"

    elif token.type == "KEY" and token.value == "continue":
        statement.data = "continue"

    elif token.type == "KEY" and token.value == "while":
        statement.data = "while"
        token = tokens.Next()
        if not (token.type == "PART" and token.value == "("):
            logger.Error("line " + str(token.line) + " expected (")
            return (False, tokens)
        expression, tokens = Parse_Expression(tokens, logger)
        if not expression:
            return (False, tokens)
        statement.Adopt(expression)
        token = tokens.Next()
        if not (token.type == "PART" and token.value == ")"):
            logger.Error("line " + str(token.line) + " expected )")
            return (False, tokens)
        body, tokens = Parse_Statement(tokens, logger)
        if not body:
            return (False, tokens)
        statement.Adopt(body)
        return (statement, tokens)

    else:
        logger.Error("line " + str(token.line) + " expected return, symbol, or int")
        return (False, tokens)

    if end:
        token = tokens.Next()
        if not token.type == "END":
            logger.Error("line " + str(token.line) + " expected END")
            for line in traceback.format_stack():
                logger.Log(line.strip())
            return (False, tokens)

    return (statement, tokens)

def Parse_Function(tokens, logger):
    params = []

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

    token = tokens.Query()
    if token.type == "KEY" and token.value == "int":
        token = tokens.Next()
        token = tokens.Next()
        if not token.type == "SYMBOL":
            logger.Error("line " + str(token.line) + " expected symbol")
            return (False, tokens)
        params.append(token.value)

        token = tokens.Query()
        while(token.type == "PART" and token.value == ","):
            token = tokens.Next()
            token = tokens.Next()
            if not (token.type == "KEY" and token.value == "int"):
                logger.Error("line " + str(token.line) + " expected int")
                return (False, tokens)
            token = tokens.Next()
            if not token.type == "SYMBOL":
                logger.Error("line " + str(token.line) + " expected symbol")
                return (False, tokens)
            params.append(token.value)
            token = tokens.Query()

    token = tokens.Next()
    if not (token.type == "PART" and token.value == ")"):
        logger.Error("line " + str(token.line) + " expected )")
        return (False, tokens)

    token = tokens.Next()
    if not (token.type == "PART" and token.value == "{"):
        logger.Error("line " + str(token.line) + " expected {")
        return (False, tokens)

    block, tokens = Parse_Block(tokens, logger, "default")
    if not block:
        return (False, tokens)

    function = Tree(name, 0)
    for param in params:
        function.GiveBirth(param)
    function.Adopt(block)

    return (function, tokens)

def Parse_Program(tokens, logger):
    program = Tree("program", 0)

    while(tokens.Len()):
        function, tokens = Parse_Function(tokens, logger)
        if not function:
            #NOTE(Noah): We are going to return false if we get a half function
            return (False, tokens)
        program.Adopt(function)

    return (program, tokens)

def Run(tokens, logger):
    program, tokens = Parse_Program(tokens, logger)
    if not program:
        return False
    return program
