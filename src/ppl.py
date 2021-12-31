# NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.

'''
Use case:
python ppl.py "inFile" -o "outFile" -t "TARGET" [options]
'''

import sys
import time
import logger
import lexer
import preparser
import syntax
import compiler
import linker

if __name__ == "__main__":
    TIMER_START = time.time()
    logger = logger.Logger()
    if len(sys.argv) > 1:
        platform = "MAC_OS" # Default platform.
        verbose = False
        inFile = sys.argv[1]
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

                            # NOTE(Noah): Here is where we call all the things to do the compilation.
                            try:
                                file = open(inFile, "r")
                                raw = file.read()
                                file.close()
                                tokens = lexer.Run(raw)
                                if verbose:
                                    logger.Log("Printing tokens, pre parser")
                                    for token in tokens.tokens:
                                        logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)
                                preparser.Run(tokens) # Directly modifies the tokens object.
                                if verbose:
                                    logger.Log("Printing tokens, post parser")
                                    for token in tokens.tokens:
                                        logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)
                                ast = syntax.Run(tokens, logger)
                                if ast and tokens.QueryNext().type == "EOL":
                                    if verbose:
                                        logger.Log("Printing AST")
                                        ast.Print(0, logger)
                                    object_file_path, r = compiler.Run(ast, outFile, logger)
                                    if r:
                                        linker.Run(object_file_path, outFile)
                                        logger.Log("Compiled to {}".format(outFile))
                                    else:
                                        logger.Error("Compilation failed.")
                                else:
                                    logger.Error("Unable to generate AST")
                            except IOError:
                                logger.Error("Unable to read {}".format(inFile))

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
    TIMER_END = time.time()
    time_elapsed_in_ms = round((TIMER_END - TIMER_START) * 1000, 2)
    logger.Log("Total time for compiler.py = " + str( time_elapsed_in_ms ) + " ms" )