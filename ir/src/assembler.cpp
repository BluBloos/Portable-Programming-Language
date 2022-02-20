// This file takes the human readable text representation of the PPL IR
// and converts it into an in-memory representation suitable for translation into
// arbitrary target platforms such as x86_64, x86, 64 bit ARM, and webassembly.

#include <ppl_util.h>

void HandleLine(char *line);

// USAGE:
// pplasm <inFile> <targetPlatform> 
int main(int argc, char **argv) {

    if (argc < 3) {
        LOGGER.Error("Not enough arguments");
        return 1;
    } else {

        char *inFilePath = argv[1];
        char *targetPlatform = argv[2];

        FILE *inFile = fopen(inFilePath, "r");

        if (inFile != NULL) {
            // To make life easy, we define that all assembly lines will be written on exactly one
            // line in the source file. The text will be ASCII encoded. There won't even be escapes to
            // allow for multi-liners.

            char *p = NULL; size_t lineCap;
            ssize_t r = getline(&p, &lineCap, inFile);

            // TODO(Noah): This code is probably going to be really slow because for every line of
            // the assembler file that we take in, we fucking allocate and dealloc memory.
            // it would be much more sensible to just make a buffer of size 1024 or smthn and just put the
            // ASCII formated lines into there.
            // and we check if the line is larger, if and only if the line is larger, then we allocate more memory.
            // but for most lines, 1024 should be good.
            while ( r != -1 ) {
                HandleLine(p);
                free(p);
                p = NULL;
                r = getline(&p, &lineCap, inFile);
            }

            // even if r is -1 (which means we reached the end of the file),
            // it could be the case that there is like one last line to handle?
            if (p != NULL) {
                HandleLine(p);
                free(p);
                p = NULL;
            }

            fclose(inFile);
        } else {
            LOGGER.Error("Unable to open inFile=%s", inFilePath);
        }

    }

    return 0;

}

void HandleLine(char *line) {
    // NOTE(Noah): For right now, we are literally just going to echo the lines of all source files.
    // And we know that all lines contain the null-terminator, so we are going to use the Min version
    // of the log function.
    LOGGER.Min("%s", line);
}