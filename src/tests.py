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
import optimization as meatBullet
init() # init the colored console stuffs
def colored(string, color):
    if color == "green":
        return Fore.GREEN + string
    elif color == "red":
        return Fore.RED + string
def SingleTest(fileName, desired_result):
    r = compiler.Run(fileName, True, "MAC_OS")
    if r:
        print(colored("{} compiled.".format(fileName), "green"))
        print(Style.RESET_ALL)
    else:
        print(colored("{} failed to compile.".format(fileName), "red"))
        print(Style.RESET_ALL)
# REGEX TREE GENERATION UNIT TEST
logger.InitLogger()
grammer = g.LoadGrammer()
for key in grammer.defs.keys():
    regex = grammer.defs[key].regExp
    regexTree = g.CreateRegexTree(grammer, regex)
    logger.Log(colored("Printing REGEX tree for r\"{}\"".format(regex), "green"))
    print(Style.RESET_ALL, end="")
    regexTree.Print(0, logger)
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
    # run an optimization pass for an optimized ast
    #ast = meatBullet.Run(ast)
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
SINGLE_TEST = True
if not SINGLE_TEST:
    try:
        dir = "design/tests/grammer"
        for fileName in os.listdir(dir):
            SingleTestAST(dir, fileName)
    except IOError:
        logger.Error("Unable to open file? :(")
else:
    SingleTestAST('design/tests/grammer', 'function3.c')
# INTEGRATION TEST FOR SINGLE GRAMMER OBJECTS
# INTEGRATION TEST FOR FULL PROGRAMS.
SingleTest("design/tests/variable_scoping.c", 3)
SingleTest("design/tests/variables.c", 5)
SingleTest("design/tests/expression.c", 1)
SingleTest("design/tests/comments.c", 0)
SingleTest("design/tests/function.c", 65)
SingleTest("design/tests/function2.c", 128)
SingleTest("design/tests/factorial.c", 6)
SingleTest("design/tests/if.c", 100)
SingleTest("design/tests/if2.c", 80)
SingleTest("design/tests/if3.c", 40)
SingleTest("design/tests/conditional.c", 12)
SingleTest("design/tests/fib.c", 13)
SingleTest("design/tests/for.c", 5)
SingleTest("design/tests/while.c", 10)
# INTEGRATION TEST FOR FULL PROGRAMS.
logger.CloseLogger()