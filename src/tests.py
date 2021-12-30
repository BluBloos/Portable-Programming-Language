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
logger.InitLogger()
try:
    dir = "design/tests/grammer"
    for fileName in os.listdir(dir):
        filePath = join(dir, fileName)
        #num = int(fileName[-3])
        grammerDefName = fileName[:-3]
        file = open(filePath, "r")
        raw = file.read()
        #TODO(Noah): What happens if the file read fails?
        file.close()
        tokens = lexer.Run(raw)
        ast = syntax.ParseTokensWithGrammer(tokens, grammer, grammer.defs[grammerDefName], logger)
        if ast:
            logger.Log("Printing AST for {}".format(filePath))
            ast.Print(0, logger)
        else:
            logger.Error("Unable to generate ast for {}".format(fileName))
except IOError:
    logger.Error("Unable to open file? :(")
logger.CloseLogger()
# INTEGRATION TEST FOR SINGLE GRAMMER OBJECTS

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