#ifndef TIMER_H
#define TIMER_H

typedef struct {
    uint64_t clocksPerSecond;
} time_glob_t;

static time_glob_t g_time;

time_glob_t *TimerGlob()
{
    return &g_time;
}

uint64_t estimate_timer_frequency()
{
    uint64_t cpuTimer = __rdtsc();
    auto begin = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds elapsed(0);
    
    // duration.
    auto osClocksForOneSecond = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::seconds(1));

    // hot loop to wait for 1 second.
    while (elapsed < osClocksForOneSecond)
    {
        auto curr = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(curr - begin);
    }

    auto cpuTimerEnd = __rdtsc();

    return cpuTimerEnd - cpuTimer;
}

class Timer {
  public:
    uint64_t begin;
    const char *msg;
    bool active;
    Timer(const char *msg) : msg(msg), active(true) {
        begin = __rdtsc();
    }
    void TimerEnd() {
        if (active) {
            uint64_t end = __rdtsc();
            uint64_t clocks = (end - begin);
            double ms = clocks / double(TimerGlob()->clocksPerSecond) * 1000.0f;
            LOGGER.Log("%s took %llu clocks and %.3f ms", msg, clocks, ms);
            active = false;
        }
    }
    ~Timer() { TimerEnd(); }
};
#endif
