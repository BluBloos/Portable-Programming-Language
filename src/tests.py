# File exists to run the tests in /tests
import compiler
import logger
from colorama import init
from colorama import Fore, Back, Style
import grammer as g
import lexer
import syntax
import os
from os.path import isfile, join

init()
def colored(string, color):
    if color == "green":
        return Fore.GREEN + string
    elif color == "red":
        return Fore.RED + string
def SingleTest(fileName, desired_result):
    compiler.Run(fileName, True, "MAC_OS")
    print(colored("{} compiled.".format(fileName), "green"))
    print(Style.RESET_ALL)

# REGEX TREE GENERATION UNIT TEST
logger.InitLogger()
grammer = g.LoadGrammer()
for key in grammer.defs.keys():
    regex = grammer.defs[key].regExp
    regexTree = g.CreateRegexTree(grammer, regex)
    logger.Log("Printing REGEX tree for r\"{}\"".format(regex))
    regexTree.Print(0, logger)
logger.CloseLogger()
# REGEX TREE GENERATION UNIT TEST

# Run integration test of lexer and ast generation on single grammer objects
def SingleTestAST(dir, fileName):
    filePath = join(dir, fileName)
    grammerDefName = fileName[:-3]
    file = open(filePath, "r")
    raw = file.read()
    #TODO(Noah): What happens if the file read fails?
    file.close()
    tokens = lexer.Run(raw)
    logger.Log("Generating AST for {}".format(filePath))
    #logger.StartInternalRecording()
    ast = syntax.ParseTokensWithGrammer(tokens, grammer, grammer.defs[grammerDefName], logger)
    #logger.StopInternalRecording()
    if ast and tokens.QueryNext().type == "EOL":
        logger.Log(colored("Printing AST for {}".format(filePath), "green"))
        print(Style.RESET_ALL, end="")
        ast.Print(0, logger)
    else:
        # We want to check if the syntax parser failed, which is to say that the code has improper grammar.
        logger.FlushLast()
        logger.ClearRecording()
        logger.Error(colored("Unable to generate ast for {}".format(fileName), "red"))
        print(Style.RESET_ALL, end="")
logger.InitLogger()
SINGLE_TEST = False
if not SINGLE_TEST:
    try:
        dir = "design/tests/grammer"
        for fileName in os.listdir(dir):
            SingleTestAST(dir, fileName)
    except IOError:
        logger.Error("Unable to open file? :(")
else:
    SingleTestAST('design/tests/grammer', 'program1.c')
logger.CloseLogger()
# INTEGRATION TEST FOR SINGLE GRAMMER OBJECTS

# TODO(Noah): Failed because the Any bit found the expression, then we tried for ;, Group failed.
# But if the Any had instead returned the var decl, we would have been good.
# Also of issue is that since the expression returned valid, we called tokens.Next()

# Possible solution might be to make it such that the Any returns a complete list of all valid things parsed.
# This is simply the issue that the beginning of the var_decl might be an expression. 
# And then we have that inside the Any.
# A good solution is to have, if you have made an Any, put the "subtring" ones LAST.
# Want to check first for the LONGEST ones.

# TODO(Noah): Add syntax errors given from the compiler. This well help us in debugging.
# I just spent a ridiculous amount of time debugging my code to figure out that the source dode
# that I was trying to compile literally had the wrong syntax, and thus the compiler claimed it could
# simply not parse the AST. It's working too well...

# TODO(Noah): for program1.c, because the function failed and it was the last...and we had the whole * (0 or more),
# because it failed, it simply didn't care because it got enough! It got some and like half.
# right so the error is because we have reached the end of the program and there remains tokens to be parsed.
# In fact, it goes exactly like this. Unless we reach the EOL token, the ast has failed?

# TODO(Noah): Last error is okay at best. Like, it's hiding the details. I know the function wasn't found,
# then it didn't find a var_decl, so the last thing it tried to do was see if what it was looking at was a struct.
# I need to be able to print 'Expected ; on line ___.' 

# Here's one way to look at it. Once we get an error like 'Expected ; on Line___'. After this point, NOTHING ELSE
# goes correctly. 

'''
Supose we are trying to get rid of error prints that occur when the lexer is "seeing" if something is there.
Ex) expected 'case'.
This is it trying to search for the 0 or more many groups, where the first thing to find is 'case'.

Let's think of when specifically we want to print that we were unable to match a character, for example.
If this character not being there results in the lexer to stop and no longer look for any more tokens.
Largely, this is in fact the very last error. So taking the last error is the exact solution...
'''

#SingleTest("design/tests/variable_scoping.c", 3)
#SingleTest("design/tests/variables.c", 5)
#SingleTest("design/tests/expression.c", 1)
#SingleTest("design/tests/comments.c", 0)
#SingleTest("design/tests/function.c", 65)
#SingleTest("design/tests/function2.c", 128)
#SingleTest("design/tests/factorial.c", 6)
#SingleTest("design/tests/if.c", 100)
#SingleTest("design/tests/if2.c", 80)
#SingleTest("design/tests/if3.c", 40)
#SingleTest("design/tests/conditional.c", 12)
#SingleTest("design/tests/fib.c", 13)
#SingleTest("design/tests/for.c", 5)
#SingleTest("design/tests/while.c", 10)


'''
TODO(Noah):

Logger stuff brainstorm

r"[(function_decl)(function_impl)(var_decl)(struct_decl)]*"

r"(type)(symbol)\(((lv),)*(lv)?\)[;(block)]"

((lv),)
Suppose we try to parse a group like this.
this is like parsing something on the regular.

like "(lv),"

so if we fail on the ,
we simply print that this is how we failed.

but if we fail on the (lv), this contains many sub-components.

r"(type)(symbol)"

can fail on either type or symbol.

type has sub-components.

r"[((symbol)::(symbol))(symbol)(keyword)]"

if type fails, (which is an Any).
- we simply say "Expected one of : ___, ___, ___"

if we fail on symbol we simply say "Expected symbol".

What happens if the components of the Any block are sufficiently advanced?
- Like we are looking at a program and it could have been either of the struct, program, or var_decl.

Maybe we have like a "best guess" type of thing. The options of the Any that got the most amount 
of tokens matched before failure. We store different error buffers for each child.

- Then we print the errors of this sub-component.

Okay now what about the modifiers???
- Gets quite complicated...
'''