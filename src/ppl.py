# NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.

'''
Use case:
python ppl.py "inFile" -o "outFile" -t "TARGET" [options]
'''

import sys
import timing
import logger as l
import lexer
import preparser
import syntax
import compiler
import linker
import os

def LexAndPreparse(inFile, logger, verbose):
    file = open(inFile, "r")
    raw = file.read()
    file.close()
    tokens = lexer.Run(raw)
    if verbose:
        logger.Log("Printing tokens, pre parser")
        for token in tokens.tokens:
            logger.Log(str(token))
    # compute cwd from inFile.
    cwd = os.path.dirname(inFile)
    pContext = preparser.Run(cwd, tokens, verbose, logger) # Directly modifies the tokens object.
    if verbose:
        logger.Log("Printing pContext")
        logger.Log("libs:"+",".join(pContext.libs))
        logger.Log("targets:"+",".join(pContext.targets))
        logger.Log("Printing tokens, post parser")
        for token in tokens.tokens:
            logger.Log(str(token))
    
    return (pContext, tokens)

def Run(inFile, outFile, platform, logger, verbose):
    # NOTE(Noah): Here is where we call all the things to do the compilation.
    try:
        pContext, tokens = LexAndPreparse(inFile, logger, verbose)
        ast = syntax.Run(tokens, logger)
        if ast and tokens.QueryNext().type == "EOL":
            if verbose:
                logger.Log("Printing AST")
                ast.Print(0, logger)
            # compilation and linker steps are where the targets start to matter
            # and we need to change behaviour accordingly.
            object_file_path, r = compiler.Run(ast, outFile, logger)
            linker.Run(pContext, object_file_path, outFile, logger)
            logger.Success("Compiled to {}".format(outFile))
            return True
        else:
            logger.Error("Unable to generate AST for {}".format(inFile))
            return False
    except IOError:
        logger.Error("Unable to read {}".format(inFile))
        return False

IN_FILE = "tests/for.c"

if __name__ == "__main__":
    timer = timing.Timer()
    logger = l.Logger()
    if len(sys.argv) > 1:
        platform = "MAC_OS" # Default platform.
        verbose = True
        #inFile = sys.argv[1]
        inFile = IN_FILE
        if len(sys.argv) > 3:
            dash_o = sys.argv[2]
            if dash_o == "-o":
                outFile = sys.argv[3]
                if len(sys.argv) > 5:
                    dash_t = sys.argv[4]
                    if dash_t == "-t":
                        # TODO(Noah): Add a hook architecture to change what the Portable Programming Language compiler
                        # does based on the target platform.
                        user_platform = sys.argv[5]
                        if user_platform == "MAC_OS":
                            Run(inFile, outFile, platform, logger, verbose)
                        else:
                            logger.Error("Platform of {} not valid".format(user_platform))
                    else:
                        logger.Error("-t expected")
                else:
                    logger.Error("No target platform specified")
            else:
                logger.Error("-o expected")
        else:
            print("No output file specified.")
    else:
        logger.Error("No input source file specified")
    timer.TimerEnd("ppl.py", logger)
    if logger.GetErrorCount() > 0:
        logger.Error( "Error count, " + str(logger.GetErrorCount()) )