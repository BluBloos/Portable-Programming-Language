#include <stdio.h>
#include <stdarg.h>
namespace ppl {
    enum Color {
        RED,
        GREEN
    };
    void print(char *fmt, ...);
    void scan(char *fmt, ...);
    void print_color(Color color, char *fmt, ...);
}