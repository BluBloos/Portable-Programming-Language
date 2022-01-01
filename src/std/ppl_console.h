#include <stdio.h>
#include <stdarg.h>
// NOTE(Noah): LMAOOOOO. Love how my string type is char *. BEAUT.
#define string char * 
namespace ppl {
    enum Color {
        RED,
        GREEN
    };
    void print(char *fmt, ...);
    void scan(char *fmt, ...);
    void print_color(Color color, char *fmt, ...);
}