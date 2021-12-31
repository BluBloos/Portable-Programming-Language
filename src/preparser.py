# TODO(Noah): There must be a way to have something approximating a struct or object literal in python.
# Is this not named tuples? But why is the syntax so poor :(
class Context:
    def __init__(self, libs, targets):
        self.libs = libs
        self.targets = targets

'''
NOTE(Noah): Since we are transpiling PPL to C, no need to touch preparse directives. C compiler will handle this.
simply handle #targets
and
- Name mangle the main entry point so that I can link it with the stub -> very stupid and silly, but just change the name of the main function.
- Change all std:: namespaces to the PPL:: namespace -> Easy a simple search and replace.
'''

'''
# Function returns a tokens list with more tokens.
# Recursively runs the lexer to generate new tokens where include directives are.
def Run(tokens):
   
    #- We want to be looking for #include statments, namely the ones for the standard libraries.
    #We simply want to note which ones that the user wishes to include.
    #- We also want to look for the #targets one to see what targets the user claims that their code works for.
    
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
            elif token.value == "#include":
                
                #Either a quote next or text between <>.
                #Find the text.
                
                text = ""
                nextToken = tokens.QueryDistance(n+1)
                if nextToken.type == "QUOTE":
                    text = nextToken.value
                elif nextToken.value == "<":
                    j = n + 2
                    while tokens.QueryDistance(j).value != ">":
                        text += tokens.QueryDistance(j).value
                        j += 1
                if "std/" in text:
                    context.libs.append(text)
        
        n += 1

    return context
'''

# Do nothing for now!
def Run(tokens):
    pass