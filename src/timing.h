#ifndef TIMER_H
#define TIMER_H
class Timer {
    public:
    clock_t begin;
    char *msg;
    bool active;
    Timer(char *msg) : msg(msg), active(true) {
        begin = clock();
    }
    void TimerEnd() {
        if (active) {
            clock_t end = clock();
            double clocks = (double)(end - begin);
            double ms = clocks / CLOCKS_PER_SEC * 1000.0f;
            LOGGER.Log("%s took %.2f clocks and %.2f ms", msg, clocks, ms);
            active = false;
        }
    }
    ~Timer() {
        TimerEnd();
    }
};
#endif