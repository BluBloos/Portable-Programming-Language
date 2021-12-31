// NOTE(Noah): No differentiation between angle brackets and "" includes. Always actually just <>.
#include <3.c> // Goal is for this to get replaced inline with all the lexed tokens of 2.c!!!
#include <stdio.h>

int main() {
    short d = MySumFun(5, 8);
    printf("%d\n", d);
}