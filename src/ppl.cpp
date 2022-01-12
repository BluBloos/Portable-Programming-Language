// NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.

/* PROJECT DEPENDENCIES */
#include <stdio.h> 
#include <string>
#include <vector>
#include <time.h>
#include <stdarg.h>
/* PROJECT DEPENDENCIES */

/* PROGRAM GLOBALS */
#include <ppl.h>
#include <logger.h>
#include <mem.h>
Logger LOGGER;
ConstMemoryArena MEMORY_ARENA(1024 * 1024 * 60); // 60 MB.
// Compiler parameters.
enum target_platform PLATFORM = POSIX;
bool VERBOSE = false;
/* PROGRAM GLOBALS */

/* OTHER COMPILATION UNITS */
#include <lexer.h>
#include <timing.h>
/* OTHER COMPILATION UNITS */

// USAGE 
// ppl.exe "inFile" -o "outFile" -t "TARGET" [options]
int main(int argc, char **argv)
{
    Timer timer = Timer("ppl.exe");

    const char *userPlatform;
    const char *inFilePath; 
    const char *outFilePath;

    bool paramsValid = false;
    if (argc > 1) {
        inFilePath = argv[1];
        if (argc > 3){
            std::string dash_o = std::string(argv[2]);
            if (dash_o == "-o") {
                outFilePath = argv[3];
                if (argc > 5){
                    std::string dash_t = std::string(argv[4]);
                    if (dash_t == "-t"){
                        userPlatform = argv[5];
                        paramsValid = true;
                    } else
                        LOGGER.Error("-t expected");
                } else
                    LOGGER.Error("No target platform specified");
            } else
                LOGGER.Error("-o expected");
        } else
            LOGGER.Error("No output file specified.");
    } else {
        LOGGER.Error("No input source file specified");
    }

    if (paramsValid) {
        
        FILE *inFile = fopen(inFilePath, "r");
        if (inFile == NULL) {
            LOGGER.Error("inFile of '%s' does not exist", inFilePath);
            return 0; // exit program.
        }
        
        TokenContainer tokensContainer;
        PreparseContext preparseContext;
        if (!LexAndPreparse(inFile, tokensContainer, preparseContext)) {
            // NOTE(Noah): exit program and silently fail. 
            // All error messages are handled by whom that throws err.
            return 0; 
        }
        if (VERBOSE) {
            tokensContainer.Print();
        }

        /*
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
        */

    }
}