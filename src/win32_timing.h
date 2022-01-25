#ifndef TIMER_H
#define TIMER_H

inline LARGE_INTEGER
Win32GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter(&Result);
	return (Result);
}

// NOTE(Noah): I stole this code from my previous PokemonDemo project.
// I have no guilt. So nostalgic, so good.
inline float
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End, LONGLONG PerfCountFrequency64)
{
	float Result = 1.0f * (End.QuadPart - Start.QuadPart) / PerfCountFrequency64;
	return (Result);
}

class Timer {
    public:
    LARGE_INTEGER LastCounter;
    LONGLONG PerfCountFrequency64;
    char *msg;
    bool active;
    Timer(char *msg) : msg(msg), active(true) {
        LARGE_INTEGER PerfCountFrequency;
        QueryPerformanceFrequency(&PerfCountFrequency);
        PerfCountFrequency64 = PerfCountFrequency.QuadPart;
        LastCounter = Win32GetWallClock();
    }
    void TimerEnd() {
        if (active) {
            float ms = 1000.0f * Win32GetSecondsElapsed(LastCounter, Win32GetWallClock(), PerfCountFrequency64);
            //clock_t end = clock();
            //double clocks = (double)(end - begin);
            //double ms = clocks / CLOCKS_PER_SEC * 1000.0f;  
            LOGGER.Log("%s took %.6f ms", msg, ms);
            active = false;
        }
    }
    ~Timer() {
        TimerEnd();
    }
};
#endif