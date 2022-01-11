// NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.

#include <stdio.h>
#include <string>

enum target_platform {
    MAC_OS,
    WINDOWS,
    POSIX, // TODO(Noah): Should this be UNIX? What is going on here?
    WEB,
    WEB_SCRIPT
};

class Logger {
    public:
    void Error(char *str) {
        printf("[ERROR]: %s\n", str);
    }
    void Log(char *str) {
        printf("[LOG]: %s\n", str);
    }
};

// Program globals.
Logger logger;

int main(int argc, char **argv)
{
    // timer = Timer()
    logger = Logger();

    // Compiler parameters.
    enum target_platform platform = POSIX;
    bool verbose = false;
    char *inFile; 
    char *outFile;
    char *userPlatform;

    bool paramsValid = false;
    if (argc > 1) {
        inFile = argv[1];
        if (argc > 3){
            std::string dash_o = std::string(argv[2]);
            if (dash_o == "-o") {
                outFile = argv[3];
                if (argc > 5){
                    std::string dash_t = std::string(argv[4]);
                    if (dash_t == "-t"){
                        userPlatform = argv[5];
                        paramsValid = true;
                    } else
                        logger.Error("-t expected");
                } else
                    logger.Error("No target platform specified");
            } else
                logger.Error("-o expected");
        } else
            logger.Error("No output file specified.");
    } else {
        logger.Error("No input source file specified");
    }

    if (paramsValid) {
        // Run the compiler.
        logger.Log("Compiling running.");
    }
}