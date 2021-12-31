import time
import logger
import lexer
import preparser
import preparser2
import codegen
import syntax
import sys

def Run(filePath, DEBUG, target):
    TIMER_START = time.time()
    try:
        file = open(filePath, "r")
        raw = file.read()
        file.close()
        tokens = lexer.Run(raw)
        if DEBUG:
            for token in tokens.tokens:
                logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)
        
        '''
        NOTE(Noah): Not sure what we were trying with preparser
        #preparser_context = preparser.Run(tokens)
        #pcontext, compiler_output = preparser2.Run(tokens)
        if DEBUG:
            logger.Log("NOW SHOWING PREPARSER OUTPUT")
            for token in compiler_output:
                logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)
        '''

        ast = syntax.Run(tokens, logger)
        if ast:
            if DEBUG:
                ast.Print(0, logger)
            output_file_path = filePath + ".output.cpp"
            codegen.Run(ast, output_file_path, logger)
        else:
            logger.Error("Unable to generate ast for {}".format(filePath))
            return False

        return True # compile success

    except IOError:
        logger.Error("Could not open/read file: " + filePath)
    TIMER_END = time.time()
    time_elapsed_in_ms = round((TIMER_END - TIMER_START) * 1000, 2)
    logger.Log("Total time for compiler.py = " + str( time_elapsed_in_ms ) + " ms" )

if __name__ == "__main__":
    if len(sys.argv) > 1:
        DEBUG = True
        platform = "MAC_OS"
        # Determine if they specified a target to compile for.
        if len(sys.argv) > 2:
            user_pla = sys.argv[2]
            if user_pla == "target=UNIX" or user_pla == "target=MAC_OS" or user_pla == "target=WEB":
                platform = user_pla
            else:
                print("Platform not recognized, defaulting to macOS.")
        else:
            print("No platform specified, defaulting to macOS.")
        # did they specify debug?
        if len(sys.argv) > 3:
            user_debug = sys.argv[3]
            if user_debug == "DEBUG=true":
                DEBUG = True
            elif user_debug == "DEBUG=false":
                DEBUG = False
        Run(sys.argv[1], DEBUG, platform)
    else:
        print("[ERROR]: No source file specified.")
    
