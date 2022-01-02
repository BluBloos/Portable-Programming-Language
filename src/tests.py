# File exists to run the tests in /tests
import compiler
import logger as l
import grammer as g
import lexer
import syntax
import timing
import os
from os.path import isfile, join
import sys
import ppl
import preparser

def SingleIntegrationTest(filePath, logger, verbose):
    outPath = filePath.replace(".c", "")
    return ppl.Run(filePath, outPath, "MAC_OS", logger, verbose)

def SingleTestAST(grammer, dir, fileName, logger):
    filePath = join(dir, fileName)
    grammerDefName = fileName[:-3]
    file = open(filePath, "r")
    raw = file.read()
    #TODO(Noah): What happens if the file read fails?
    file.close()
    tokens = lexer.Run(raw)
    logger.Log("Generating AST for {}".format(filePath))
    ast = syntax.ParseTokensWithGrammer(tokens, grammer, grammer.defs[grammerDefName], logger)
    if ast and tokens.QueryNext().type == "EOL":
        logger.Success("Printing AST for {}".format(filePath))
        ast.Print(0, logger)
        return True
    else:
        logger.Error("Unable to generate ast for {}".format(fileName))
        return False

# TESTING CHOICE.
TEST = "all"

if __name__ == "__main__":
    timer = timing.Timer()
    logger = l.Logger()
    verbose = False

    errors = 0

    _tests = [TEST]
    if TEST == "all":
        _tests = ["integration", "regex_gen", "grammers", "preparser"]

    for test in _tests:

        if test == "integration":
            # Check for file to run through ppl toolchain
            # TODO(Noah): Add target platform testing for this.
            try:
                dir = "tests/"
                for fileName in os.listdir(dir):
                    path = dir + fileName
                    if isfile(path) and path.endswith(".c"):
                        result = SingleIntegrationTest(path, logger, verbose)
                        if not result:
                            errors += 1
            except IOError as e:
                errors += 1
                logger.Error(str(e))

        elif test == "regex_gen":
            # REGEX TREE GENERATION UNIT TEST
            grammer = g.LoadGrammer()
            for key in grammer.defs.keys():
                regex = grammer.defs[key].regExp
                # TODO(Noah): What if we are unable to print a regex tree?
                regexTree = g.CreateRegexTree(grammer, regex)
                logger.Success("Printing REGEX tree for r\"{}\"".format(regex))
                regexTree.Print(0, logger)
            # REGEX TREE GENERATION UNIT TEST

        elif test == "grammers":
            grammer = g.LoadGrammer()
            try:
                dir = "tests/grammer"
                for fileName in os.listdir(dir):
                    result = SingleTestAST(grammer, dir, fileName, logger)
                    if not result: errors += 1
            except IOError as e:
                logger.Error(str(e))
                errors += 1

        elif test == "preparser":
            try:
                dir = "tests/preparse"
                for fileName in os.listdir(dir):
                    logger.Success("Testing preparser for {}".format(fileName))
                    inFile = join(dir, fileName)  
                    pContext, tokens = ppl.LexAndPreparse(inFile, logger, True)
            except IOError as e:
                logger.Error(str(e))
                errors += 1

    timer.TimerEnd("tests.py", logger)
    if errors > 0:
        logger.Error("Completed with {} error(s)".format(errors))
    else:
        logger.Success("Completed with 0  errors.")
