class Logger {
    public:
    void Error(char *fmt, ...) {
        printf("[ERROR]: ");
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
    void Log(char *fmt, ...) {
        printf("[LOG]: ");
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
    // prints minimally.
    void Min(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
};