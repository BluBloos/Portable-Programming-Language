class Logger {
    public:
    FILE *logFile;
    const char *logFileName = "log.txt";
    void InitFileLogging(char *mode) {
        if (logFile == NULL)
            logFile = fopen(logFileName, mode);
    }
    ~Logger() {
        if (logFile != NULL)
            fclose(logFile);
    }
    void _print(char *prefix, char *fmt, va_list args) {
        va_list _args;
        va_copy(_args, args);
        printf(prefix);
        if (logFile != NULL)
            fprintf(logFile, prefix);
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
    }
    void Success(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        _print("[SUCCESS]:", fmt, args);
        va_end (args);
    }
    void Error(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        _print("[ERROR]:", fmt, args);
        va_end (args);
    }
    void Log(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        _print("[LOG]:", fmt, args);
        va_end (args);
    }
    // prints minimally, and does not put a newline.
    void Min(char *fmt, ...) {
        va_list args;
        va_list _args;
        va_start (args, fmt);
        va_copy(_args, args);
        vprintf(fmt, args);
        if (logFile != NULL) {
            vfprintf(logFile, fmt, _args);
            va_end(_args);
        }
        va_end (args);
    }
};