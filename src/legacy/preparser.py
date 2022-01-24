import os
from os.path import isfile, join
import ppl

# TODO(Noah): There must be a way to have something approximating a struct or object literal in python.
# Is this not named tuples? But why is the syntax so poor :(
class Context:
    def __init__(self, libs, targets):
        self.libs = libs
        self.targets = targets
        self.failFlag = False

'''
TODO(Noah): 
Implement MACROS and other C preparser things!
'''

'''
TODO(Noah): Since we are transpiling PPL to C, no need to touch preparse directives. C compiler will handle this.
simply handle #targets
and
- Function returns a tokens list with more tokens
    - recursively runs the lexer to generate new tokens where include directives are #TODO 
- Name mangle the main entry point so that I can link it with the stub -> very stupid and silly, but just change the name of the main function.
'''
def Run(cwd, tokens, verbose, logger):
   
    #- We want to be looking for #include statments, namely the ones for the standard libraries.
    #We simply want to note which ones that the user wishes to include.
    #- We also want to look for the #targets one to see what targets the user claims that their code works for.
    
    newTokens = []
    context = Context([], [])
    
    n = 0
    while n < len(tokens.tokens):
        token = tokens.QueryDistance(n)
        if token.type == "PRE":
            if token.value == "#targets":
                #Add all symbols until a newline
                lineNumber = token.line
                j = n + 1
                while tokens.QueryDistance(j).line == lineNumber:
                    token = tokens.QueryDistance(j)
                    if (token.type == "SYMBOL"):
                        context.targets.append(token.value)
                    j += 1
                n = j # skip to the new line and resume there.
            elif token.value == "#include":
                #Either a quote next or text between <>.
                #Find the text.
                text = ""
                nextToken = tokens.QueryDistance(n+1)
                #if nextToken.type == "QUOTE":
                    #text = nextToken.value
                if nextToken.value == "<":
                    j = n + 2
                    while tokens.QueryDistance(j).value != ">":
                        text += tokens.QueryDistance(j).value
                        j += 1
                else:
                    # BAD!
                    context.failFlag = True
                    logger.Error("Line {}. Expected <".format(nextToken.line))
                    break

                # Check for the lib as a local PPL file. If so, we need to include this proper as lexed tokens.
                # TODO(Noah): Add list of directories for includes so we can do -I dir on the command line!
                # NOTE(Noah): Right now we only support the cwd.
                filePath = join(cwd, text)
                # TODO(Noah): Prevent the recursive bullshit that happens if someone tries to include a file
                # that is literally the current file. This is an infinite #include...
                if os.path.exists(filePath):
                    pContext, _tokens = ppl.LexAndPreparse(filePath, logger, verbose)
                    # Steal libs from here to add.
                    for lib in pContext.libs:
                        if lib not in context.lins:
                            context.libs.append(lib)
                    # Insert the tokens inline.
                    newTokens += _tokens.tokens
                else:
                    # NOTE(Noah): Passthru standard libraries.
                    context.libs.append(text) # Append all libraries to include.
                    #logger.Error("{} does not exist".format(filePath))
                    #context.failFlag = True
                    #break
                
                n = j # advance n because we want to skip over all tokens for this #include.
        else:
            # default case is to simply pass through the token
            newTokens.append(token)

        n += 1
    
    # TODO(Noah): Is there a fast way to do this?
    if not context.failFlag:
        tokens.tokens = newTokens # assign new tokens arr.

    return context
