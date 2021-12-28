import time
BEGIN = time.time()

import logger
import lexer
import syntax
import semantics
import codegen
import optimization

import sys
import os
import subprocess

def Run(fileName, DEBUG, TEST, platform):

    file = open(fileName, "r")
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
    file = open(fileName + ".asm", "w")
    codegen.Run(platform, tree, file)
    file.close()
    #instructions = optimization.Run(instructions)

    #TODO(Noah): There is more room for error here
    #write out assembler instructions

    # Assemble and link the code (using 3rd party software)
    exe_name = "{}.exe".format(fileName)

    if (platform == "WINDOWS"):
        subprocess.run(["nasm", "-fwin32", "{}.asm".format(fileName)])
        subprocess.run(["link", "/subsystem:console", "/entry:start" ,
                        "{}.obj".format(fileName), "/OUT:{}".format(exe_name)])
    elif (platform == "LINUX"):
        pass

    return_val = 0

    if TEST:
        if os.path.isfile(exe_name):
            if DEBUG:
                print("Running {}".format(exe_name))
            result = subprocess.run([exe_name])
            return_val = result.returncode
            if DEBUG:
                print("Return Code: {}".format(result.returncode))

    END = time.time()
    ELAPSED = round((END - BEGIN) * 1000, 3)
    logger.Log("Elapsed: " + str(ELAPSED) + "ms")

    return return_val



if __name__ == "__main__":
    if len(sys.argv) > 1:

        platform = "WINDOWS"
        DEBUG = True

        # Determine if they specified a specific platform
        if len(sys.argv) > 2:
            user_pla = sys.argv[2]
            if user_pla == "LINUX" or user_pla == "WINDOWS":
                platform = user_pla
            else:
                print("Platform not recognized, defaulting to Windows.")
        else:
            print("No platform specified, defaulting to Windows.")

        # did they specify debug?
        if len(sys.argv) > 3:
            user_debug = sys.argv[3]
            if user_debug == "DEBUG=true":
                DEBUG = True
            elif user_debug == "DEBUG=false":
                DEBUG = False

        Run(sys, DEBUG, True, platform)
    else:
        logger.Error("No source file.")
