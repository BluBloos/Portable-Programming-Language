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
    ppl.Run(filePath, outPath, "MAC_OS", logger, verbose)

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
    else:
        logger.Error("Unable to generate ast for {}".format(fileName))

if __name__ == "__main__":
    timer = timing.Timer()
    logger = l.Logger()
    verbose = False
    if len(sys.argv) > 1:
        # go the command
        command = sys.argv[1]
        if command == "integration":
            # Check for file to run through ppl toolchain
            # TODO(Noah): Add target platform testing for this.
            try:
                dir = "tests/"
                for fileName in os.listdir(dir):
                    path = dir + fileName
                    if isfile(path) and path.endswith(".c"):
                        SingleIntegrationTest(path, logger, verbose)
            except IOError as e:
                logger.Error(str(e))
        elif command == "regex_gen":
            # REGEX TREE GENERATION UNIT TEST
            grammer = g.LoadGrammer()
            for key in grammer.defs.keys():
                regex = grammer.defs[key].regExp
                # TODO(Noah): What if we are unable to print a regex tree?
                regexTree = g.CreateRegexTree(grammer, regex)
                logger.Success("Printing REGEX tree for r\"{}\"".format(regex))
                regexTree.Print(0, logger)
            # REGEX TREE GENERATION UNIT TEST
        elif command == "grammers":
            grammer = g.LoadGrammer()
            try:
                dir = "tests/grammer"
                for fileName in os.listdir(dir):
                    SingleTestAST(grammer, dir, fileName, logger)
            except IOError as e:
                logger.Error(str(e))
        elif command == "preparser":
            try:
                dir = "tests/preparse"
                for fileName in os.listdir(dir):
                    logger.Log("Testing preparser for {}".format(fileName))
                    inFile = join(dir, fileName)  
                    pContext, tokens = ppl.LexAndPreparse(inFile, logger, True)
            except IOError as e:
                logger.Error(str(e))

    else:
        pass # silently fail / do nothing.
    timer.TimerEnd("tests.py", logger)