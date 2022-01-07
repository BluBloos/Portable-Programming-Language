#ifndef PPL_TIME
#define PPL_TIME
#include <time.h>
namespace ppl {
    // returns seconds since EPOCH.
    unsigned int _time() {
         time_t t = time(NULL);
        // NOTE(Noah): Apprently the C standard does not define time_t, but that it is probably
        // the seconds since epoch...
        // see https://en.cppreference.com/w/c/chrono/time_t
        return (unsigned int)t; 
    }
}
#endif