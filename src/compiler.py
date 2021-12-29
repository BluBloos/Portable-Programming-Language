import time
TIMER_START = time.time()
import logger
import lexer
import sys
def Run(filePath, DEBUG, target):
    try:
        file = open(filePath, "r")
        raw = file.read()
        file.close()
        tokens = lexer.Run(raw)
        if DEBUG:
            for token in tokens.tokens:
                logger.Log("TYPE: " + token.type + ", VALUE: " + token.value)
    except IOError:
        logger.Error("Could not open/read file: " + filePath)
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
        logger.Error("No source file specified.")
