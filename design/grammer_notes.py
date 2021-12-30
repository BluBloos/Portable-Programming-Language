'''FACTOR
    Might be any of
    - literal
    - variable
    - function call
    - unary operator on a factor.
    - might be paranthesis around an expression
    - might be literals like "true" and "false.
'''

'''SWITCH
    - begins with keyword switch
    - then you get (
    - then you have like either an expression or simply a symbol
    - close bracket )
    - then open bracket {
    - a list of the following group
        - keyword case
        - an expression
        - followed by ':'
        - then you have a list of statements
            - potentially with a break statement in there
    - then you might have the following group
        - keyword default
        - followed by ':'
        - a list of statements
    - close bracket }
    '''

'''IF
    - first we expect (
    - then some sort of expression
    - then another )
    - then a statement
    - then there may or may not be the word else
        - in which case if there is, we expect a statement (do something, which may be another if)
        - i.e. else if statement.
'''

'''STATEMENT
    could be a return statement
    could be a variable decl
    could be an if statement
    could be a for statement
    could be a while loop statement
    could be a switch statement
    could be an expression
    could be a keyword
        continue/break
    could be a code block

some of these end with semi-colon and others do not.

'''

'''TYPE
    - types might be keywords, Ex) int
    - types may be user defined, Ex) window
    - types may be from a namespace, Ex) std::Terminal
'''

'''FUNCTION
    For function decl grammer
    - We expect a return type
    - then a symbol (the function name)
    - then the open bracket
    - a list of parameters
    - the close bracket
    - a block of code OR an endline
'''

'''STRUCT_DECL
    - keyword struct
    - {
    - a list of variable declarations
    - }
    - ;
    '''