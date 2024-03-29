// NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.
#include <ppl_core.h>

// USAGE
// ppl.exe "inFile" -o "outFile" -t "TARGET" [options]
int main(int argc, char **argv) {
    Timer timer = Timer("ppl.exe");
    LOGGER.InitFileLogging("w");

    const char *userPlatform;
    const char *inFilePath;
    const char *outFilePath;

    bool paramsValid = false;
    if (argc > 1) {
        inFilePath = argv[1];
        if (argc > 3) {
            std::string dash_o = std::string(argv[2]);
            if (dash_o == "-o") {
                outFilePath = argv[3];
                if (argc > 5) {
                    std::string dash_t = std::string(argv[4]);
                    if (dash_t == "-t") {
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

        LoadGrammar();

        FILE *inFile = fopen(inFilePath, "r");
        if (inFile == NULL) {
            LOGGER.Error("inFile of '%s' does not exist", inFilePath);
            return 0; // exit program.
        }

        TokenContainer tokensContainer;
        if (!Lex(inFile, tokensContainer)) {
            // NOTE(Noah): exit program and silently fail.
            // All error messages are handled by whom that throws err.

            // TODO(Noah): Add some sort of file manager object because then we won't
            // have to write fclose twice.
            fclose(inFile);
            return 0;
        }

        fclose(inFile);

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

    timer.TimerEnd();
}