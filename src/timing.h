class Timer {
    public:
    clock_t begin;
    char *msg;
    Timer(char *msg) : msg(msg) {
        begin = clock();
    }
    ~Timer() {
        clock_t end = clock();
        double clocks = (double)(end - begin);
        double ms = clocks / CLOCKS_PER_SEC * 1000.0f;
        LOGGER.Log("%s took %.2f ms", msg, clocks, ms);
    }

};