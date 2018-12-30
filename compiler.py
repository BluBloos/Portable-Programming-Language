import time
BEGIN = time.time()

DEBUG = True

import sys
import logger
import lexer
import syntax
import semantics
import codegen
import optimization
from subprocess import call

if len(sys.argv) > 1:

    file = open(sys.argv[1], "r")
    raw = file.read()

    #TODO(Noah): What happens if the file read fails?
    file.close()

    tokens = lexer.Run(raw)

    if DEBUG:
        for token in tokens.tokens:
            logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)

    tree = syntax.Run(tokens, logger)

    if not tree:
        sys.exit()

    if DEBUG:
        tree.Print(0)
        #tree.PrintWeights(0)
        #print("Depth: " + str(tree.depth))

    #annotatedTree, result = semantics.Run(tree, logger)

    #if result == False:
    #    sys.exit()
    file = open(sys.argv[1] + ".asm", "w")
    codegen.Run(tree, file)
    file.close()
    #instructions = optimization.Run(instructions)

    #TODO(Noah): There is more room for error here
    #write out assembler instructions

    #assemble the code
    #call(["nasm", "-O binary " +  sys.argv[1] + ".asm"])

else:
    logger.Error("No source file.")

END = time.time()
ELAPSED = round((END - BEGIN) * 1000, 3)
logger.Log("Elapsed: " + str(ELAPSED) + "ms")
