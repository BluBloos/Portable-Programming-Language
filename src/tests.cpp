/*
// File exists to run the tests in /tests
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
*/

/*
def SingleIntegrationTest(filePath, logger, verbose):
    outPath = filePath.replace(".c", "")
    return ppl.Run(filePath, outPath, "MAC_OS", logger, verbose)
*/

/*
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
*/

// Standard for any compilation unit of this project.
#include <ppl.h>

enum ppl_test {
    PTEST_PREPARSER_SINGLE = 0,
    PTEST_INTEGRATION,
    PTEST_REGEX_GEN,
    PTEST_REGEX_GEN_SINGLE,
    PTEST_GRAMMER,
    PTEST_GRAMMER_SINGLE,
    PTEST_PREPARSER,
    PTEST_COUNT, // comes after all tests
    PTEST_ALL // comes after count
};

// global variables
enum ppl_test TEST = PTEST_PREPARSER_SINGLE;
char* TEST_UNIT = "1.c";

int main(int argc, char **argv) {
    
    Timer timer = Timer("tests.exe");
    int errors = 0;

    std::vector<enum ppl_test> _tests;
    _tests.push_back(TEST);

    if (TEST == PTEST_ALL) {
        _tests(PTEST_COUNT);
        for (int i = 0; i < PTEST_COUNT; i++)
            _tests[i] = i;
    }

    for (int i = 0; i < _tests.size(); i++) {
        enum ppl_test test = _tests[i];
        switch(test) {
            case PTEST_INTEGRATION:
            {
                /*# Check for file to run through ppl toolchain
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
                */
            }
            break;
            case PTEST_REGEX_GEN:
            {
                /*# REGEX TREE GENERATION UNIT TEST
                grammer = g.LoadGrammer()
                for key in grammer.defs.keys():
                    regex = grammer.defs[key].regExp
                    # TODO(Noah): What if we are unable to print a regex tree?
                    regexTree = g.CreateRegexTree(grammer, regex)
                    logger.Success("Printing REGEX tree for r\"{}\"".format(regex))
                    regexTree.Print(0, logger)
                # REGEX TREE GENERATION UNIT TEST
                */
            }
            break;
            case PTEST_REGEX_GEN_SINGLE:
            {
                /*try:
                    grammer = g.LoadGrammer()
                    regex = grammer.defs[SINGLE_UNIT].regExp
                    regexTree = g.CreateRegexTree(grammer, regex)
                    logger.Success("Printing REGEX tree for r\"{}\"".format(regex))
                    regexTree.Print(0, logger)
                except:
                    logger.Error("Unable to generate REGEX tree for r\"{}\"".format(regex))
                    errors += 1
                */
            }
            break;
            case PTEST_GRAMMER:
            {
                /*
                grammer = g.LoadGrammer()
                try:
                    dir = "tests/grammer"
                    for fileName in os.listdir(dir):
                        result = SingleTestAST(grammer, dir, fileName, logger)
                        if not result: errors += 1
                except IOError as e:
                    logger.Error(str(e))
                    errors += 1
                */
            }
            break;
            case PTEST_GRAMMER_SINGLE:
            {

            }
            break;
            case PTEST_PREPARSER:
            {

            }
            break;
            case PTEST_PREPARSER_SINGLE:
            {
                
            }
            break;
        }
    }

    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}