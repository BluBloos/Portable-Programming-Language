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
    // everything is a single test (because we invoke tests.exe many times with different parameters).
    PTEST_PREPARSER = 0,
    PTEST_INTEGRATION,
    PTEST_REGEX_GEN,
    PTEST_GRAMMER,
    PTEST_COUNT, // comes after all tests
    PTEST_ALL // comes after count
};

// global variables
// enum ppl_test TEST = PTEST_PREPARSER_SINGLE;
// char* TEST_UNIT = "1.c";

// usage tests.exe testType fileName
int main(int argc, char **argv) {
    
    Timer timer = Timer("tests.exe");
    int errors = 0;

    /*
    std::vector<enum ppl_test> _tests;
    _tests.push_back(TEST);

    if (TEST == PTEST_ALL) {
        _tests(PTEST_COUNT);
        for (int i = 0; i < PTEST_COUNT; i++)
            _tests[i] = i;
    }*/

    if (argc > 2) {

        enum ppl_test test;
        char *rtest = argv[1];

        // I dunno, first thought is to do an if-statement switch type thing.
        // but fuck, we can actually do a switch on the first character.
        switch(*rtest) {
            case 'p': case 'P':
            test = PTEST_PREPARSER;
            break;
            case 'i': case 'I':
            test = PTEST_INTEGRATION;
            break;
            case 'r': case 'R':
            test = PTEST_REGEX_GEN;
            break;
            case 'g': case 'G':
            test = PTEST_GRAMMER;
            break;
        }

        char *inFilePath = argv[2];
        
        switch(test) {
            case PTEST_INTEGRATION:
            break;
            case PTEST_REGEX_GEN:
            break;
            case PTEST_GRAMMER:
            break;
            case PTEST_PREPARSER:
            {
                FILE *inFile = fopen(inFilePath, "r");
                if (inFile == NULL) {
                    LOGGER.Error("inFile of '%s' does not exist", inFilePath);
                    errors += 1;
                } else {
                    TokenContainer tokensContainer;
                    PreparseContext preparseContext;
                    if (LexAndPreparse(inFile, tokensContainer, preparseContext)) {
                        if (VERBOSE) {
                            tokensContainer.Print();
                        }
                    } else {
                        LOGGER.Error("LexAndPreparse failed.");
                        errors += 1;
                    }
                }
                fclose(inFile);
            }
            break;
        }
    } else {
        LOGGER.Error("tests.exe expects at least 2 parameters, but got none.");
        errors += 1;
    }

    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}