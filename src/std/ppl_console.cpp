#include "ppl_console.h"
void ppl::print(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
}
void ppl::scan(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vscanf(fmt, args);
        va_end (args);
}
void ppl::print_color(ppl::Color color, char *fmt, ...) {
        // Set color
        switch(color) {
                case ppl::RED:
                printf("\033[0;31m");
                break;
                case ppl::GREEN:
                printf("\033[0;32m");
                break;
        }
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        // reset the color
        printf("\033[0m");
}