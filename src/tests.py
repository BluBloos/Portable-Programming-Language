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
    ast, err = syntax.ParseTokensWithGrammer(tokens, grammer, grammer.defs[grammerDefName], logger)
    if ast and tokens.QueryNext().type == "EOL":
        logger.Success("Printing AST for {}".format(filePath))
        ast.Print(0, logger)
        return True
    else:

        if len(err) > 0:
            tokenIndex = max(error.token_index for error in err)
            for error in err:
                if error.token_index == tokenIndex:
                    logger.Error(str(error))

        logger.Error("Unable to generate ast for {}".format(fileName))
        return False

# TESTING CHOICE.
# TODO(Noah): Test grammers here because we got the good old maximum recursion depth :(
TEST = "integration"
SINGLE_UNIT = "program2.c"

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
        
        elif test == "regex_gen_single":
            try:
                grammer = g.LoadGrammer()
                regex = grammer.defs[SINGLE_UNIT].regExp
                regexTree = g.CreateRegexTree(grammer, regex)
                logger.Success("Printing REGEX tree for r\"{}\"".format(regex))
                regexTree.Print(0, logger)
            except:
                logger.Error("Unable to generate REGEX tree for r\"{}\"".format(regex))
                errors += 1


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

        elif test == "grammers_single":
            try:
                grammer = g.LoadGrammer()
                dir = "tests/grammer"
                result = SingleTestAST(grammer, dir, SINGLE_UNIT, logger)
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
        elif test == "preparser_single":
            try:
                dir = "tests/preparse"
                logger.Success("Testing preparser for {}".format(SINGLE_UNIT))
                inFile = join(dir, SINGLE_UNIT)
                pContext, tokens = ppl.LexAndPreparse(inFile, logger, True)  
            except IOError as e:
                logger.Error(str(e))
                errors += 1

    timer.TimerEnd("tests.py", logger)
    if errors > 0:
        logger.Error("Completed with {} error(s)".format(errors))
    else:
        logger.Success("Completed with 0  errors.")
