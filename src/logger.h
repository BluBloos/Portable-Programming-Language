enum Color { RED, GREEN, WHITE };

// TODO: maybe we should flush to file because until I exit
// the build program the stuff in log.txt does not output.
// that is sad.

class Logger {
  public:

    FILE *logFile;
    const char *logFileName = "log.txt";

    struct {
        char *currFile; // the full file-path.
    } logContext;

    void InitFileLogging(char *mode) {
        if (logFile != NULL) {
            fclose(logFile);
        }
        logFile = fopen(logFileName, mode);
    }
    ~Logger() {
        if (logFile != NULL)
            fclose(logFile);
    }
    void _print(Color color, char *prefix, char *fmt, va_list args) {
        switch (color) {
            case RED: printf("\033[0;31m"); break;
            case GREEN: printf("\033[0;32m"); break;
            default: // do nothing.
                break;
        }
        va_list _args;
        va_copy(_args, args);
        printf("%s", prefix);
        if (logFile != NULL)
            fprintf(logFile, "%s", prefix);
        //va_list args;
        //va_start (args, fmt);
        vprintf(fmt, args);
        //va_end (args);
        if (logFile != NULL) {
            vfprintf(logFile, fmt, _args);
            va_end(_args);
            //va_end (args);
        }
        printf("\n");
        if (logFile != NULL)
            fprintf(logFile, "\n");
        // clear color
        printf("\033[0m");
    }
    void Success(char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        _print(GREEN, "[SUCCESS]:", fmt, args);
        va_end(args);
    }
    void Error(char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        _print(RED, "[ERROR]:", fmt, args);
        va_end(args);
    }
    void Log(char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        _print(WHITE, "[LOG]:", fmt, args);
        va_end(args);
    }

    // TODO: make this work safely for multiple threads or something like that.
    //
    void EmitUserError(
        const char *file, uint32_t line, uint32_t c, 
        const char *codeContext, const char *niceDesc, ...
    ) {

        Error(""); // so we just want the error in RED + the newline.

        constexpr auto bufSize = 1024;
        char prefixBuf[bufSize];
        snprintf(prefixBuf, bufSize, "%s:%u,%u: ", file, line, c); 

        va_list args;
        va_start(args, niceDesc);
        // TODO: maybe make print take in const.
        _print(WHITE, prefixBuf, (char *)niceDesc, args);
        va_end(args);

        Min(ColorHighlight);
        Min("\n%s\n\n", codeContext);
        Min(ColorNormal);
    }

    // prints minimally, and does not put a newline.
    void Min(char *fmt, ...) {
        va_list args;
        va_list _args;
        va_start(args, fmt);
        va_copy(_args, args);
        vprintf(fmt, args);
        if (logFile != NULL) {
            vfprintf(logFile, fmt, _args);
            va_end(_args);
        }
        va_end(args);
    }
};