class Timer {
    public:
    clock_t begin;
    char *msg;
    Timer(char *msg) : msg(msg) {
        begin = clock();
    }
    ~Timer() {
        clock_t end = clock();
        double cycles = (double)(end - begin);
        double ms = cycles / CLOCKS_PER_SEC * 1000.0f;
        LOGGER.Log("%s took %.2f cyles and %.2f ms", msg, cycles, ms);
    }

};