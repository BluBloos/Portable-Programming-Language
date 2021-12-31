#include "ppl_console.h"
void ppl::print(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
}